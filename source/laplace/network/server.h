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
#include "session_pool.h"
#include "stats.h"
#include <utility>

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

    log_handler log = get_global_log();

    server(server const &) = delete;
    server(server &&)      = delete;
    server &operator=(server const &) = delete;
    server &operator=(server &&) = delete;

    server() noexcept = default;
    ~server() noexcept;

    void setup_protocol(protocol_interface const &interface) noexcept;
    void setup_execution(
        execution_interface const &interface) noexcept;

    void enable_encryption(bool is_enabled) noexcept;
    void set_max_slot_count(sl::whole count) noexcept;
    void set_token(span_cbyte token) noexcept;

    auto listen(std::span<endpoint_info const> endpoints) noexcept
        -> coroutine::task<>;
    auto connect(connect_info const &info) noexcept
        -> coroutine::task<>;

    void queue(span_cbyte seq) noexcept;
    void time_elapsed(sl::time delta_msec) noexcept;

    [[nodiscard]] auto get_port(sl::index io = 0) const noexcept
        -> uint16_t;
    [[nodiscard]] auto get_token() const noexcept -> span_cbyte;
    [[nodiscard]] auto has_token() const noexcept -> bool;

    [[nodiscard]] auto is_connected() const noexcept -> bool;
    [[nodiscard]] auto is_quit() const noexcept -> bool;

    auto await_listen(
        std::span<endpoint_info const> endpoints) noexcept
        -> coroutine::task<>;
    auto await_listen(endpoint_info const &info) -> coroutine::task<>;
    auto await_connect(connect_info const &info) -> coroutine::task<>;

  private:
    static auto _await(auto awaitable) -> coroutine::task<>;

    struct endpoint {
      ptr_io   io;
      ptr_node node;
    };

    void tick() noexcept;

    void read_endpoints() noexcept;
    void read_endpoint(endpoint const &ep) noexcept;
    void process_requests(endpoint const        &ep,
                          std::span<vbyte const> reqs) noexcept;
    void process_request(endpoint const &ep, span_cbyte req) noexcept;
    void read_sessions() noexcept;
    void process_session(span_cbyte req) noexcept;
    void session_open(endpoint const &ep) noexcept;
    void setup_cipher(span_cbyte req);
    void send_session_request(endpoint const  &ep,
                              std::string_view address,
                              uint16_t         port) noexcept;
    void set_remote_endpoint(span_cbyte req) noexcept;
    void set_remote_key(span_cbyte req) noexcept;
    void send_session_response(ptr_node const &node) noexcept;
    void send_request_token(ptr_node const &node) noexcept;
    void send_session_token(ptr_node const &node) noexcept;

    protocol_interface   m_proto;
    sl::vector<endpoint> m_endpoints;
    random               m_random;
    transfer             m_tran;
    bool                 m_is_connected = false;
    ptr_node             m_session_node;
    std::string          m_remote_address;
    uint16_t             m_remote_port = any_port;
    vbyte                m_session_token;
  };

  using ptr_server = std::shared_ptr<server>;
}

#include "server.impl.h"

#endif
