/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_SLOT_POOL_H
#define LAPLACE_NETWORK_SLOT_POOL_H

#include "session_state.h"

namespace laplace::network {
  class session_pool {
  public:
    enum find_tag { find_default, add_if_not_found };

    log_handler log = get_global_log();

    void set_max_count(sl::whole count) noexcept;

    void clear() noexcept;
    void erase_disconnected() noexcept;

    auto add(std::string_view address, uint16_t port) noexcept
        -> sl::index;

    [[nodiscard]] auto get_count() const noexcept -> sl::whole;
    [[nodiscard]] auto has(sl::index n) const noexcept -> bool;

    [[nodiscard]] auto find(std::string_view address, uint16_t port,
                            find_tag tag = find_default) noexcept
        -> sl::index;

    [[nodiscard]] auto find_token(span_cbyte token) const noexcept
        -> sl::index;

    [[nodiscard]] auto is_full() const noexcept -> bool;

    [[nodiscard]] auto operator[](sl::index slot) noexcept
        -> session_state &;
    [[nodiscard]] auto operator[](sl::index slot) const noexcept
        -> session_state const &;

  private:
    sl::whole                 m_max_count = slot_count_unlimited;
    sl::vector<session_state> m_slots;
  };
}

#endif
