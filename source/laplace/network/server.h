/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_SERVER_H
#define LAPLACE_NETWORK_SERVER_H

#include "clock.h"
#include "interface/execution.h"
#include "interface/protocol.h"
#include "random.h"
#include "slot_pool.h"
#include "stats.h"

namespace laplace::network {
  class server {
  public:
    struct endpoint {
      ptr_io   io;
      uint16_t port;
    };

    struct connect_info {
      ptr_io           io;
      std::string_view host_address;
      uint16_t         host_port;
      uint16_t         client_port = any_port;
    };

    server(server const &) = delete;
    server(server &&)      = delete;
    server &operator=(server const &) = delete;
    server &operator=(server &&) = delete;

    server() noexcept = default;
    ~server() noexcept;

    void setup_protocol(protocol_interface const &interface) noexcept;
    void setup_execution(
        execution_interface const &interface) noexcept;
    void setup_log(log_interface const &interface) noexcept;

    void enable_encryption(bool is_enabled) noexcept;
    void set_max_slot_count(sl::whole count) noexcept;

    void listen(std::span<endpoint const> endpoints) noexcept;
    void connect(connect_info info) noexcept;

    void queue(span_cbyte seq) noexcept;
    void tick(sl::time delta_msec) noexcept;

    [[nodiscard]] auto get_port(sl::index io = 0) const noexcept
        -> uint16_t;

    [[nodiscard]] auto is_connected() const noexcept -> bool;
    [[nodiscard]] auto is_quit() const noexcept -> bool;

    inline void listen(endpoint const &ep) noexcept {
      this->listen({ &ep, 1 });
    }

  private:
    struct endpoint_internal {
      ptr_io   io;
      ptr_node node;
    };

    void read_endpoints() noexcept;
    void read_endpoint(endpoint_internal const &ep) noexcept;
    void process_requests(endpoint_internal const &ep,
                          std::span<vbyte const>   reqs) noexcept;
    void process_request(endpoint_internal const &ep,
                         span_cbyte               req) noexcept;
    void session_open(endpoint_internal const &ep,
                      span_cbyte               req) noexcept;

    protocol_interface            m_proto;
    sl::vector<endpoint_internal> m_endpoints;
    transfer                      m_tran;
  };

  using ptr_server = std::shared_ptr<server>;
}

#endif
