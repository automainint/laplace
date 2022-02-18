/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "session_pool.h"

#include "../core/string.h"

namespace laplace::network {
  using std::string;

  void session_pool::set_max_count(sl::whole count) noexcept {
    m_max_count = count;
  }

  void session_pool::clear() noexcept {
    m_slots.clear();
  }

  void session_pool::erase_disconnected() noexcept {
    m_slots.erase(
        std::remove_if(m_slots.begin(), m_slots.end(),
                       [](auto const &s) { return !s.is_connected; }),
        m_slots.end());
  }

  auto session_pool::add(std::string_view address,
                         uint16_t port) noexcept -> sl::index {
    sl::index const id = m_slots.size();
    m_slots.emplace_back<session_state>(
        { .address = string { address }, .port = port });
    return id;
  }

  auto session_pool::get_count() const noexcept -> sl::whole {
    return m_slots.size();
  }

  auto session_pool::has(sl::index n) const noexcept -> bool {
    return n >= 0 && n < m_slots.size();
  }

  auto session_pool::find(std::string_view address, uint16_t port,
                          find_tag tag) noexcept -> sl::index {

    for (sl::index i = 0; i < m_slots.size(); i++)
      if (m_slots[i].address == address && m_slots[i].port == port)
        return i;

    if (tag == find_default) {
      log(log_event::verbose,
          fmt("Reject %s:%d. Joining is disabled.", address.data(),
              (int) port),
          "Network/Session Pool");
      return -1;
    }

    if (is_full()) {
      log(log_event::verbose,
          fmt("Reject %s:%d. No free slots.", address.data(),
              (int) port),
          "Network/Session Pool");
      return -1;
    }

    return add(address, port);
  }

  auto session_pool::find_token(span_cbyte token) const noexcept
      -> sl::index {
    auto equal = [](span_cbyte left, span_cbyte right) -> bool {
      if (left.size() != right.size())
        return false;
      for (sl::index i = 0; i < left.size(); i++)
        if (left[i] != right[i])
          return false;
      return true;
    };

    if (token.empty())
      return index_undefined;

    for (sl::index i = 0; i < m_slots.size(); i++)
      if (equal(m_slots[i].token, token))
        return i;

    return index_undefined;
  }

  auto session_pool::is_full() const noexcept -> bool {
    return m_max_count >= 0 && m_slots.size() <= m_max_count;
  }

  auto session_pool::operator[](sl::index slot) noexcept
      -> session_state & {
    if (slot < 0 || slot >= m_slots.size()) {
      log(log_event::error, "Invalid slot index.", __FUNCTION__);
      static session_state invalid;
      return invalid;
    }

    return m_slots[slot];
  }

  auto session_pool::operator[](sl::index slot) const noexcept
      -> session_state const & {
    if (slot < 0 || slot >= m_slots.size()) {
      log(log_event::error, "Invalid slot index.", __FUNCTION__);
      static session_state const invalid;
      return invalid;
    }

    return m_slots[slot];
  }
}
