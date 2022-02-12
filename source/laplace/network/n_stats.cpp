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
    m_log.print(fmt("Total bytes sent:      %d", (int) m_total_sent));
    m_log.print(
        fmt("Total bytes received:  %d", (int) m_total_received));

    if (m_total_received > 0) {
      m_log.print(
          fmt("Corruption:  %d bytes (%d%%)", (int) m_total_loss,
              (int) (m_total_loss * 100 + m_total_received / 2) /
                  m_total_received));
    }
  }

  void stats::set_log_interface(log_interface const &in) noexcept {
    if (!check_log_interface(in)) {
      error_("Invalid log interface.", __FUNCTION__);
      return;
    }

    m_log = in;
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
