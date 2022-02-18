/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "stats.h"

#include "../core/string.h"

namespace laplace::network {
  stats::~stats() noexcept {
    log(log_event::verbose,
        fmt("Total bytes sent:      %d", (int) m_total_sent),
        "Network/Stats");
    log(log_event::verbose,
        fmt("Total bytes received:  %d", (int) m_total_received),
        "Network/Stats");

    if (m_total_received > 0)
      log(log_event::verbose,
          fmt("Corruption:  %d bytes (%d%%)", (int) m_total_loss,
              (int) (m_total_loss * 100 + m_total_received / 2) /
                  m_total_received),
          "Network/Stats");
  }

  void stats::tick() noexcept {
    m_total_sent += m_bytes_sent;
    m_total_received += m_bytes_received;
    m_total_loss += m_bytes_loss;

    m_bytes_sent     = 0;
    m_bytes_received = 0;
    m_bytes_loss     = 0;
  }

  void stats::add_bytes_sent(sl::whole count) noexcept {
    m_bytes_sent += count;
  }

  void stats::add_bytes_received(sl::whole count) noexcept {
    m_bytes_received += count;
  }

  void stats::add_bytes_loss(sl::whole count) noexcept {
    m_bytes_loss += count;
  }

  auto stats::get_bytes_sent() const noexcept -> sl::whole {
    return m_bytes_sent;
  }

  auto stats::get_bytes_received() const noexcept -> sl::whole {
    return m_bytes_received;
  }

  auto stats::get_bytes_loss() const noexcept -> sl::whole {
    return m_bytes_loss;
  }
}
