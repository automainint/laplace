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
#include "interface/socket.h"
#include "stats.h"
#include "transfer.h"
#include <random>

namespace laplace::network {
  class server {
  public:
    static constexpr sl::index slot_host            = -1;
    static constexpr sl::index slot_count_unlimited = -1;
    static constexpr sl::index id_undefined         = -1;
    static constexpr sl::index index_undefined      = -1;
    static constexpr sl::time  time_undefined       = -1;

    static sl::time const  default_update_timeout_msec;
    static sl::time const  default_ping_timeout_msec;
    static sl::time const  default_connection_timeout_msec;
    static sl::whole const chunk_size;
    static sl::whole const default_loss_compensation;
    static sl::index const max_index_delta;

    server(server const &) = delete;
    server(server &&)      = delete;
    auto operator=(server const &) -> server & = delete;
    auto operator=(server &&) -> server & = delete;

    server() noexcept;
    ~server() noexcept;

    void setup_protocol_interface(
        protocol_interface const &pin) noexcept;
    void setup_execution_interface(
        execution_interface const &ein) noexcept;
    void setup_log_interface(log_interface const &lin) noexcept;

    void set_encryption_enabled(bool is_enabled) noexcept;
    void set_max_slot_count(sl::whole count) noexcept;

    void listen(uint16_t port) noexcept;
    void connect(std::string_view host_address, uint16_t host_port,
                 uint16_t client_port) noexcept;
    void queue(span_cbyte seq) noexcept;
    void tick(sl::time delta_msec) noexcept;

    [[nodiscard]] auto get_port() const noexcept -> uint16_t;

    [[nodiscard]] auto is_connected() const noexcept -> bool;

  private:
    enum class server_state { prepare, action, pause };
    enum class event_status { proceed, remove };

    struct event_queue {
      sl::index         index = 0;
      sl::vector<vbyte> events;
    };

    struct slot_info {
      std::string address;
      uint16_t    port = any_port;
      vbyte       token;

      sl::index                id_actor     = id_undefined;
      bool                     is_connected = true;
      transfer::encryption_tag encryption   = transfer::plain;
      bool                     is_exclusive = false;
      bool                     request_flag = true;
      sl::time                 outdate      = 0;
      sl::time                 wait         = 0;

      sl::vector<vbyte> in;
      sl::vector<vbyte> out;

      event_queue queue;
      transfer    tran;

      std::unique_ptr<socket_interface::node> node;
    };

    [[nodiscard]] auto get_state() const noexcept -> server_state;

    [[nodiscard]] auto perform_control(sl::index  slot,
                                       span_cbyte seq) noexcept
        -> event_status;

    [[nodiscard]] auto do_request_events(sl::index  slot,
                                         span_cbyte seq) noexcept
        -> event_status;
    [[nodiscard]] auto do_request_token(sl::index slot) noexcept
        -> event_status;
    [[nodiscard]] auto do_session_request(sl::index  slot,
                                          span_cbyte seq) noexcept
        -> event_status;
    [[nodiscard]] auto do_session_response(sl::index  slot,
                                           span_cbyte seq) noexcept
        -> event_status;
    [[nodiscard]] auto do_session_token(sl::index  slot,
                                        span_cbyte seq) noexcept
        -> event_status;
    [[nodiscard]] auto do_ping_request(sl::index  slot,
                                       span_cbyte seq) noexcept
        -> event_status;
    [[nodiscard]] auto do_ping_response(sl::index  slot,
                                        span_cbyte seq) noexcept
        -> event_status;
    [[nodiscard]] auto do_server_heartbeat(sl::index  slot,
                                           span_cbyte seq) noexcept
        -> event_status;
    [[nodiscard]] auto do_server_init() noexcept -> event_status;
    [[nodiscard]] auto do_server_loading() noexcept -> event_status;
    [[nodiscard]] auto do_server_action() noexcept -> event_status;
    [[nodiscard]] auto do_server_pause() noexcept -> event_status;
    [[nodiscard]] auto do_server_reserve(span_cbyte seq) noexcept
        -> event_status;
    [[nodiscard]] auto do_server_clock(span_cbyte seq) noexcept
        -> event_status;
    [[nodiscard]] auto do_server_seed(span_cbyte seq) noexcept
        -> event_status;
    [[nodiscard]] auto do_server_quit() noexcept -> event_status;
    [[nodiscard]] auto do_client_enter(sl::index slot) noexcept
        -> event_status;
    [[nodiscard]] auto do_client_leave(sl::index slot) noexcept
        -> event_status;
    [[nodiscard]] auto do_client_ready(sl::index  slot,
                                       span_cbyte seq) noexcept
        -> event_status;

    void set_connected(bool is_connected) noexcept;

    void set_state(server_state state) noexcept;

    [[nodiscard]] auto get_connection_timeout() const noexcept
        -> sl::time;

    [[nodiscard]] auto get_update_timeout() const noexcept
        -> sl::time;
    [[nodiscard]] auto get_ping_timeout() const noexcept -> sl::time;

    [[nodiscard]] auto is_encryption_enabled() const noexcept -> bool;
    [[nodiscard]] auto is_master() const noexcept -> bool;

    void reset_connection() noexcept;

    void setup() noexcept;
    void cleanup() noexcept;

    void update_world() noexcept;

    void send_events() noexcept;

    void send_event_to(sl::index slot, span_cbyte seq) noexcept;
    void send_event(span_cbyte seq) noexcept;

    void append_event(sl::index slot, span_cbyte seq) noexcept;

    void set_master(bool is_master) noexcept;

    auto add_slot(std::string_view address, uint16_t port) noexcept
        -> sl::index;

    [[nodiscard]] auto find_slot(std::string_view address,
                                 uint16_t port) noexcept -> sl::index;

    void process_slots() noexcept;
    void process_queue(sl::index slot) noexcept;
    void check_outdate(sl::index slot) noexcept;
    void clean_slots() noexcept;
    void process_event(sl::index slot, span_cbyte seq) noexcept;
    void distribute_event(sl::index slot, span_cbyte seq) noexcept;
    void add_instant_event(span_cbyte seq) noexcept;
    void perform_event(sl::index slot, span_cbyte seq) noexcept;
    void perform_instant_events() noexcept;

    void receive_events() noexcept;
    void process_buffers() noexcept;
    void receive_chunks() noexcept;

    void receive_from(sl::index slot) noexcept;
    void process_chunk(sl::index slot, span_cbyte chunk) noexcept;
    void send_event_history_to(sl::index slot) noexcept;

    void add_event(sl::index slot, span_cbyte seq) noexcept;
    auto encode_chunk(sl::index slot) noexcept -> vbyte;
    void send_chunk(sl::index slot, span_cbyte chunk) noexcept;
    void send_chunks() noexcept;
    void disconnect(sl::index slot) noexcept;

    void update_slots(sl::time delta_msec) noexcept;

    [[nodiscard]] auto has_free_slots() const noexcept -> bool;
    [[nodiscard]] auto generate_token() noexcept -> vbyte;

    clock               m_clock;
    stats               m_stats;
    protocol_interface  m_proto = blank_protocol_interface();
    execution_interface m_exe   = blank_execution_interface();
    log_interface       m_log   = blank_log_interface();

    bool m_is_connected = false;

    sl::time m_connection_timeout_msec =
        default_connection_timeout_msec;
    sl::time m_update_timeout_msec = default_update_timeout_msec;
    sl::time m_ping_timeout_msec   = default_ping_timeout_msec;

    server_state m_state = server_state::prepare;

    std::vector<slot_info>                  m_slots;
    std::unique_ptr<socket_interface::node> m_node;
    std::unique_ptr<socket_interface>       m_socket_interface;

    vbyte       m_buffer = vbyte(chunk_size);
    event_queue m_queue;

    sl::vector<vbyte> m_instant_events;

    bool      m_is_master             = false;
    bool      m_is_encryption_enabled = true;
    sl::whole m_max_slot_count        = slot_count_unlimited;
    sl::whole m_loss_compensation     = default_loss_compensation;
    sl::time  m_ping_clock            = {};
    vbyte     m_token;

    std::mt19937_64 m_rand;
    uint64_t        m_seed = {};
  };

  using ptr_server = std::shared_ptr<server>;
}

#endif
