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

#include "../coroutine/task.h"
#include "clock.h"
#include "interface/execution.h"
#include "interface/protocol.h"
#include "random.h"
#include "slot_pool.h"
#include "stats.h"

namespace laplace::network {
  class server {
  public:
    struct endpoint_info {
      ptr_io   io;
      uint16_t port;
    };

    struct connect_info {
      ptr_io      io;
      std::string host_address;
      uint16_t    host_port;
      uint16_t    client_port = any_port;
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

    auto listen(std::span<endpoint_info const> endpoints) noexcept
        -> coroutine::task<>;
    auto connect(connect_info info) noexcept -> coroutine::task<>;

    void queue(span_cbyte seq) noexcept;
    void time_elapsed(sl::time delta_msec) noexcept;

    [[nodiscard]] auto get_port(sl::index io = 0) const noexcept
        -> uint16_t;

    [[nodiscard]] auto is_connected() const noexcept -> bool;
    [[nodiscard]] auto is_quit() const noexcept -> bool;

    inline auto await_listen(
        std::span<endpoint_info const> endpoints) noexcept
        -> coroutine::task<> {
      return _await(this->listen(endpoints));
    }

    inline auto await_listen(endpoint_info const &ep)
        -> coroutine::task<> {
      return await_listen(sl::vector<endpoint_info> { ep });
    }

    inline auto await_connect(connect_info info)
        -> coroutine::task<> {
      return _await(this->connect(info));
    }

  private:
    static inline auto _await(auto awaitable) -> coroutine::task<> {
      awaitable.resume();
      return awaitable;
    }

    struct endpoint {
      ptr_io   io;
      ptr_node node;
    };

    void read_endpoints() noexcept;
    void read_endpoint(endpoint const &ep) noexcept;
    void process_requests(endpoint const        &ep,
                          std::span<vbyte const> reqs) noexcept;
    void process_request(endpoint const &ep, span_cbyte req) noexcept;
    void session_open(endpoint const &ep, span_cbyte req) noexcept;

    protocol_interface   m_proto;
    sl::vector<endpoint> m_endpoints;
    transfer             m_tran;
  };

  using ptr_server = std::shared_ptr<server>;
}

#endif
