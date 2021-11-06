/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "world.h"

#include "basic_impact.h"
#include <algorithm>
#include <utility>

namespace laplace::engine {
  using std::make_shared, std::unique_lock, std::shared_lock,
      std::adopt_lock;

  bool const     world::default_allow_relaxed_spawn = false;
  uint64_t const world::default_seed                = 0xfeedbeef;

  world::world(world &&other) noexcept {
    auto _ul = unique_lock(other.m_lock);

    locked_assign(std::move(other));

    m_scheduler = make_unique<scheduler>(*this);
  }

  auto world::operator=(world &&other) noexcept -> world & {
    lock(m_lock, other.m_lock);
    auto _ul       = unique_lock(m_lock, adopt_lock);
    auto _ul_other = unique_lock(other.m_lock, adopt_lock);

    locked_assign(std::move(other));

    m_scheduler = make_unique<scheduler>(*this);

    return *this;
  }

  world::world() noexcept {
    m_scheduler = make_unique<scheduler>(*this);
  }

  world::~world() noexcept {
    m_scheduler->set_done();
    m_scheduler->join();
  }

  auto world::reserve(sl::index id) noexcept -> sl::index {
    return spawn(make_shared<basic_entity>(), id);
  }

  void world::emplace(ptr_entity const &ent, sl::index id) noexcept {
    if (!ent) {
      error_("Unable to spawn null entity.", __FUNCTION__);
      return;
    }

    auto _ul = unique_lock(m_lock);

    if (id == id_undefined)
      id = m_next_id;

    if (id >= m_entities.size())
      m_entities.resize(id + 1);

    if (m_entities[id]) {
      if (m_entities[id]->is_dynamic())
        locked_erase_dynamic(id);

      m_entities[id]->reset_world();
    }

    m_entities[id] = ent;

    ent->set_world(shared_from_this(), id);

    while (m_next_id < m_entities.size() && m_entities[m_next_id])
      m_next_id++;

    if (ent->is_dynamic())
      locked_add_dynamic(id);
  }

  auto world::spawn(ptr_entity const &ent, sl::index id) noexcept
      -> sl::index {
    if (!ent) {
      error_("Unable to spawn null entity.", __FUNCTION__);
      return id_undefined;
    }

    auto _ul = unique_lock(m_lock);

    if (id == id_undefined)
      id = m_next_id;

    if (id >= m_entities.size())
      m_entities.resize(id + 1);

    if (m_entities[id]) {
      if (!m_allow_relaxed_spawn) {
        error_("Id is not free.", __FUNCTION__);
        locked_desync();

        return id_undefined;
      }

      if (m_entities[id]->is_dynamic())
        locked_erase_dynamic(id);

      m_entities[id]->reset_world();
    }

    m_entities[id] = ent;

    ent->set_world(shared_from_this(), id);

    while (m_next_id < m_entities.size() && m_entities[m_next_id])
      m_next_id++;

    if (ent->is_dynamic())
      locked_add_dynamic(id);

    return id;
  }

  void world::remove(sl::index id) noexcept {
    auto _ul = unique_lock(m_lock);

    if (id < m_entities.size()) {
      if (m_entities[id]) {
        if (m_entities[id]->is_dynamic()) {
          locked_erase_dynamic(id);
        }

        m_entities[id]->reset_world();
        m_entities[id].reset();
      } else {
        if (m_allow_relaxed_spawn) {
          error_("No entity.", __FUNCTION__);
          locked_desync();
        }
      }

      if (m_next_id > id) {
        m_next_id = id;
      }
    } else {
      error_("Invalid entity id.", __FUNCTION__);
      locked_desync();
    }
  }

  void world::respawn(sl::index id) noexcept {
    auto _ul = unique_lock(m_lock);

    if (id < m_entities.size()) {
      if (m_entities[id]) {
        locked_erase_dynamic(id);

        if (m_entities[id]->is_dynamic()) {
          locked_add_dynamic(id);
        }
      }
    }
  }

  void world::clear() noexcept {
    auto _ul = unique_lock(m_lock);

    for (auto &ent : m_entities) {
      if (ent) {
        ent->reset_world();
      }
    }

    m_entities.clear();
    m_dynamic_ids.clear();

    m_root    = id_undefined;
    m_next_id = 0;
    m_desync  = false;
  }

  void world::queue(ptr_impact ev) noexcept {
    if (ev) {
      if (ev->is_async()) {
        auto _ul = unique_lock(m_lock);
        m_queue.emplace_back(std::move(ev));

      } else {
        auto _ul = unique_lock(m_lock);

        auto iter = lower_bound(
            m_sync_queue.begin(), m_sync_queue.end(), ev->get_order(),
            [](ptr_impact const &a, cref_eventorder b) -> bool {
              return a->get_order() < b;
            });

        m_sync_queue.emplace(iter, std::move(ev));
      }
    }
  }

  void world::tick(sl::time delta) noexcept {
    schedule(delta);

    join();
  }

  void world::schedule(sl::time delta) noexcept {
    if (get_thread_count() > 0) {
      m_scheduler->schedule(delta);
      return;
    }

    auto _ul = unique_lock(m_lock);

    for (sl::time t = 0; t < delta; t++) {
      while (!m_sync_queue.empty() || !m_queue.empty()) {
        for (auto &i : m_sync_queue) {
          _ul.unlock();

          i->perform({ *this, access::sync });

          _ul.lock();
        }

        m_sync_queue.clear();

        for (auto &i : m_queue) {
          _ul.unlock();

          i->perform({ *this, access::async });

          _ul.lock();
        }

        m_queue.clear();
      }

      for (auto id : m_dynamic_ids) {
        _ul.unlock();

        auto &en = m_entities[id];

        if (en->clock())
          en->tick({ *this, access::async });

        _ul.lock();
      }

      for (auto &e : m_entities)
        if (e)
          e->adjust();
    }
  }

  void world::join() noexcept {
    if (m_scheduler)
      m_scheduler->join();
  }

  void world::set_thread_count(
      sl::whole const thread_count) noexcept {
    if (check_scheduler()) {
      m_scheduler->set_thread_count(thread_count);
    }
  }

  auto world::get_thread_count() noexcept -> sl::whole {
    if (check_scheduler()) {
      return m_scheduler->get_thread_count();
    }
    return 0;
  }

  void world::set_root(sl::index id_root) noexcept {
    auto _ul = unique_lock(m_lock);
    m_root   = id_root;
  }

  auto world::get_root() noexcept -> sl::index {
    auto _sl = shared_lock(m_lock);
    return m_root;
  }

  void world::allow_relaxed_spawn(bool is_allowed) noexcept {
    auto _ul = unique_lock(m_lock);

    m_allow_relaxed_spawn = is_allowed;
  }

  auto world::is_relaxed_spawn_allowed() noexcept -> bool {
    auto _sl = shared_lock(m_lock);
    return m_allow_relaxed_spawn;
  }

  auto world::get_random() noexcept -> ref_rand {
    return m_rand;
  }

  auto world::get_entity(sl::index id) noexcept -> ptr_entity {
    auto _sl = shared_lock(m_lock);

    if (id < 0 || id >= m_entities.size()) {
      return {};
    }

    return m_entities[id];
  }

  void world::desync() noexcept {
    auto _ul = unique_lock(m_lock);
    locked_desync();
  }

  auto world::is_desync() noexcept -> bool {
    auto _sl = shared_lock(m_lock);
    return m_desync;
  }

  auto world::check_scheduler() noexcept -> bool {
    auto _sl = shared_lock(m_lock);
    return m_scheduler ? true : false;
  }

  void world::clean_sync_queue() noexcept {
    auto _ul = unique_lock(m_lock);

    if (m_index <= m_sync_queue.size()) {
      m_sync_queue.erase(m_sync_queue.begin(),
                         m_sync_queue.begin() +
                             static_cast<ptrdiff_t>(m_index));

      m_index = 0;
    }
  }

  void world::clean_async_queue() noexcept {
    auto _ul = unique_lock(m_lock);

    if (m_index <= m_queue.size()) {
      m_queue.erase(m_queue.begin(),
                    m_queue.begin() +
                        static_cast<ptrdiff_t>(m_index));

      m_index = 0;
    }
  }

  void world::reset_index() noexcept {
    auto _ul = unique_lock(m_lock);
    m_index  = 0;
  }

  auto world::no_queue() noexcept -> bool {
    auto _sl = shared_lock(m_lock);
    return m_sync_queue.empty() && m_queue.empty();
  }

  auto world::next_sync_impact() noexcept -> ptr_impact {
    auto _ul = unique_lock(m_lock);

    return m_index < m_sync_queue.size()
               ? std::move(m_sync_queue[m_index++])
               : ptr_impact {};
  }

  auto world::next_async_impact() noexcept -> ptr_impact {
    auto _ul = unique_lock(m_lock);

    return m_index < m_queue.size() ? std::move(m_queue[m_index++])
                                    : ptr_impact();
  }

  auto world::next_dynamic_entity() noexcept -> ptr_entity {
    auto _ul = unique_lock(m_lock);

    return m_index < m_dynamic_ids.size()
               ? m_entities[m_dynamic_ids[m_index++]]
               : ptr_entity();
  }

  auto world::next_entity() noexcept -> ptr_entity {
    auto _ul = unique_lock(m_lock);

    while (m_index < m_entities.size()) {
      auto &en = m_entities[m_index++];

      if (en)
        return en;
    }

    return {};
  }

  void world::locked_desync() noexcept {
    m_desync = true;
    verb(" :: DESYNC");
  }

  void world::locked_add_dynamic(sl::index id) noexcept {
    auto it = lower_bound(m_dynamic_ids.begin(), m_dynamic_ids.end(),
                          id);

    m_dynamic_ids.emplace(it, id);
  }

  void world::locked_erase_dynamic(sl::index id) noexcept {
    auto it = lower_bound(m_dynamic_ids.begin(), m_dynamic_ids.end(),
                          id);

    if (it != m_dynamic_ids.end() && *it == id) {
      m_dynamic_ids.erase(it);
    }
  }

  void world::locked_assign(world &&other) noexcept {
    m_allow_relaxed_spawn = other.m_allow_relaxed_spawn;
    m_desync              = other.m_desync;
    m_root                = other.m_root;
    m_next_id             = other.m_next_id;
    m_index               = other.m_index;
    m_rand                = other.m_rand;

    m_dynamic_ids = std::move(other.m_dynamic_ids);
    m_entities    = std::move(other.m_entities);
    m_queue       = std::move(other.m_queue);
    m_sync_queue  = std::move(other.m_sync_queue);
  }
}
