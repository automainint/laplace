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

#include "interface/log.h"
#include "slot_state.h"

namespace laplace::network {
  class slot_pool {
  public:
    enum find_tag { find_default, add_if_not_found };

    void set_log_interface(log_interface const &lin) noexcept;
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

    [[nodiscard]] auto is_full() const noexcept -> bool;

    [[nodiscard]] auto operator[](sl::index slot) noexcept
        -> slot_state &;
    [[nodiscard]] auto operator[](sl::index slot) const noexcept
        -> slot_state const &;

  private:
    log_interface          m_log       = blank_log_interface();
    sl::whole              m_max_count = slot_count_unlimited;
    sl::vector<slot_state> m_slots;
  };
}

#endif
