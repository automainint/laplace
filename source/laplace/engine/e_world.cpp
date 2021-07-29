/*  laplace/engine/e_world.cpp
 *
 *  Copyright (c) 2021 Mitya Selivanov
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

namespace laplace::engine {
  using std::make_shared, std::unique_lock, std::shared_lock;

  const bool world::default_allow_relaxed_spawn = false;

  world::world() {
    m_scheduler = make_unique<scheduler>(*this);
  }

  auto world::reserve(sl::index id) -> sl::index {
    return spawn(make_shared<basic_entity>(), id);
  }

  void world::emplace(ptr_entity ent, sl::index id) {
    auto _ul = unique_lock(m_lock);

    if (ent) {
      if (id == id_undefined) {
        id = m_next_id;
      }

      if (id >= m_entities.size()) {
        m_entities.resize(id + 1);
      }

      if (m_entities[id]) {
        if (m_entities[id]->is_dynamic()) {
          locked_erase_dynamic(id);
        }

        m_entities[id]->reset_world();
      }

      m_entities[id] = ent;

      ent->set_id(id);
      ent->set_world(shared_from_this());

      while (m_next_id < m_entities.size() && m_entities[m_next_id]) {
        m_next_id++;
      }

      if (ent->is_dynamic()) {
        locked_add_dynamic(id);
      }
    } else {
      error_("Unable to spawn null entity.", __FUNCTION__);
    }
  }

  auto world::spawn(ptr_entity ent, sl::index id) -> sl::index {
    auto _ul = unique_lock(m_lock);

    if (ent) {
      if (id == id_undefined) {
        id = m_next_id;
      }

      if (id >= m_entities.size()) {
        m_entities.resize(id + 1);
      }

      if (m_entities[id]) {
        if (!m_allow_relaxed_spawn) {
          error_("Id is not free.", __FUNCTION__);
          locked_desync();

          return id_undefined;
        }

        if (m_entities[id]->is_dynamic()) {
          locked_erase_dynamic(id);
        }

        m_entities[id]->reset_world();
      }

      m_entities[id] = ent;

      ent->set_id(id);
      ent->set_world(shared_from_this());

      while (m_next_id < m_entities.size() && m_entities[m_next_id]) {
        m_next_id++;
      }

      if (ent->is_dynamic()) {
        locked_add_dynamic(id);
      }

      return id;
    } else {
      error_("Unable to spawn null entity.", __FUNCTION__);
    }

    return id_undefined;
  }

  void world::remove(sl::index id) {
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

  void world::respawn(sl::index id) {
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

  void world::clear() {
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

  void world::queue(ptr_impact ev) {
    if (ev) {
      if (ev->is_async()) {
        auto _ul = unique_lock(m_lock);

        m_queue.emplace_back(ev);
      } else {
        auto _ul = unique_lock(m_lock);

        auto op = [](const ptr_impact &a, cref_eventorder b) -> bool {
          return a->get_order() < b;
        };

        auto iter = lower_bound(m_sync_queue.begin(),
                                m_sync_queue.end(), ev->get_order(),
                                op);

        m_sync_queue.emplace(iter, ev);
      }
    }
  }

  void world::tick(uint64_t delta) {
    schedule(delta);

    join();
  }

  void world::schedule(uint64_t delta) {
    if (m_scheduler) {
      if (m_scheduler->get_thread_count() <= 0) {
        auto _ul = unique_lock(m_lock);

        for (uint64_t t = 0; t < delta; t++) {
          while (!m_sync_queue.empty() || !m_queue.empty()) {
            for (sl::index i = 0; i < m_sync_queue.size(); i++) {
              _ul.unlock();

              m_sync_queue[i]->perform({ *this, access::sync });

              _ul.lock();
            }

            m_sync_queue.clear();

            for (sl::index i = 0; i < m_queue.size(); i++) {
              _ul.unlock();

              m_queue[i]->perform({ *this, access::async });

              _ul.lock();
            }

            m_queue.clear();
          }

          for (sl::index i = 0; i < m_dynamic_ids.size(); i++) {
            _ul.unlock();

            auto &en = m_entities[m_dynamic_ids[i]];

            if (en->clock())
              en->tick({ *this, access::async });

            _ul.lock();
          }

          for (auto &e : m_entities) {
            if (e)
              e->adjust();
          }
        }

      } else {
        m_scheduler->schedule(delta);
      }
    }
  }

  void world::join() {
    if (m_scheduler) {
      m_scheduler->join();
    }
  }

  void world::set_thread_count(const sl::whole thread_count) {
    if (check_scheduler()) {
      m_scheduler->set_thread_count(thread_count);
    }
  }

  auto world::get_thread_count() -> sl::whole {
    if (check_scheduler()) {
      return m_scheduler->get_thread_count();
    }
    return 0;
  }

  void world::set_root(sl::index id_root) {
    auto _ul = unique_lock(m_lock);
    m_root   = id_root;
  }

  auto world::get_root() -> sl::index {
    auto _sl = shared_lock(m_lock);
    return m_root;
  }

  void world::allow_relaxed_spawn(bool is_allowed) {
    auto _ul = unique_lock(m_lock);

    m_allow_relaxed_spawn = is_allowed;
  }

  auto world::is_relaxed_spawn_allowed() -> bool {
    auto _sl = shared_lock(m_lock);
    return m_allow_relaxed_spawn;
  }

  auto world::get_random() -> ref_rand {
    return m_rand;
  }

  auto world::get_entity(sl::index id) -> ptr_entity {
    auto _sl = shared_lock(m_lock);

    if (id < 0 || id >= m_entities.size()) {
      return {};
    }

    return m_entities[id];
  }

  void world::desync() {
    auto _ul = unique_lock(m_lock);
    locked_desync();
  }

  auto world::is_desync() -> bool {
    auto _sl = shared_lock(m_lock);
    return m_desync;
  }

  auto world::check_scheduler() -> bool {
    auto _sl = shared_lock(m_lock);
    return m_scheduler ? true : false;
  }

  void world::locked_desync() {
    m_desync = true;
    verb(" :: DESYNC");
  }

  void world::locked_add_dynamic(sl::index id) {
    auto it = lower_bound(m_dynamic_ids.begin(), m_dynamic_ids.end(),
                          id);

    m_dynamic_ids.emplace(it, id);
  }

  void world::locked_erase_dynamic(sl::index id) {
    auto it = lower_bound(m_dynamic_ids.begin(), m_dynamic_ids.end(),
                          id);

    if (it != m_dynamic_ids.end() && *it == id) {
      m_dynamic_ids.erase(it);
    }
  }

  void world::clean_sync_queue() {
    auto _ul = unique_lock(m_lock);

    if (m_index <= m_sync_queue.size()) {
      m_sync_queue.erase(m_sync_queue.begin(),
                         m_sync_queue.begin() +
                             static_cast<ptrdiff_t>(m_index));

      m_index = 0;
    }
  }

  void world::clean_async_queue() {
    auto _ul = unique_lock(m_lock);

    if (m_index <= m_queue.size()) {
      m_queue.erase(m_queue.begin(),
                    m_queue.begin() + static_cast<ptrdiff_t>(m_index));

      m_index = 0;
    }
  }

  void world::reset_index() {
    auto _ul = unique_lock(m_lock);
    m_index  = 0;
  }

  auto world::no_queue() -> bool {
    auto _sl = shared_lock(m_lock);
    return m_sync_queue.empty() && m_queue.empty();
  }

  auto world::next_sync_impact() -> ptr_impact {
    auto _ul = unique_lock(m_lock);

    return m_index < m_sync_queue.size() ? m_sync_queue[m_index++]
                                         : ptr_impact();
  }

  auto world::next_async_impact() -> ptr_impact {
    auto _ul = unique_lock(m_lock);

    return m_index < m_queue.size() ? m_queue[m_index++] : ptr_impact();
  }

  auto world::next_dynamic_entity() -> ptr_entity {
    auto _ul = unique_lock(m_lock);

    return m_index < m_dynamic_ids.size()
               ? m_entities[m_dynamic_ids[m_index++]]
               : ptr_entity();
  }

  auto world::next_entity() -> ptr_entity {
    auto _ul = unique_lock(m_lock);

    while (m_index < m_entities.size()) {
      auto &en = m_entities[m_index++];

      if (en)
        return en;
    }

    return {};
  }
}
