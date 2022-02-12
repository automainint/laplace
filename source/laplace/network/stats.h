/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_STATS_H
#define LAPLACE_NETWORK_STATS_H

#include "interface/log.h"

namespace laplace::network {
  class stats {
  public:
    stats(stats const &) = delete;
    stats(stats &&)      = default;
    auto operator=(stats const &) -> stats & = delete;
    auto operator=(stats &&) -> stats & = delete;

    stats() = default;
    ~stats() noexcept;

    void set_log_interface(log_interface const &in) noexcept;

    void tick() noexcept;

    void add_bytes_sent(sl::whole count) noexcept;
    void add_bytes_received(sl::whole count) noexcept;
    void add_bytes_loss(sl::whole count) noexcept;

    [[nodiscard]] auto get_bytes_sent() const noexcept -> sl::whole;
    [[nodiscard]] auto get_bytes_received() const noexcept
        -> sl::whole;
    [[nodiscard]] auto get_bytes_loss() const noexcept -> sl::whole;

  private:
    log_interface m_log = blank_log_interface();

    sl::whole m_bytes_sent     = 0;
    sl::whole m_bytes_received = 0;
    sl::whole m_bytes_loss     = 0;

    sl::whole m_total_sent     = 0;
    sl::whole m_total_received = 0;
    sl::whole m_total_loss     = 0;
  };
}

#endif
