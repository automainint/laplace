/*  laplace/network/udp_server.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_network_udp_server_h
#define laplace_network_udp_server_h

#include "../engine/defs.h"
#include "../engine/loader.h"
#include "server.h"
#include "transfer.h"
#include "udp_node.h"

namespace laplace::network {
  /*  Base class for host and
   *  remote servers.
   */
  class udp_server : public server {
  public:
    static constexpr sl::index slot_host            = -1;
    static constexpr sl::index slot_count_unlimited = -1;

    static const sl::whole default_chunk_size;
    static const sl::whole chunk_size_increment;
    static const sl::whole chunk_size_limit;
    static const sl::whole default_loss_compensation;
    static const uint16_t  default_max_command_id;
    static const sl::index max_index_delta;

    ~udp_server() override;

    void set_encryption_enabled(bool is_enabled) noexcept;
    void set_allowed_commands(span_cuint16 commands);

    void set_chunk_size(sl::whole size);

    void queue(span_cbyte seq) override;
    void tick(sl::time delta_msec) override;

    [[nodiscard]] auto get_port() const -> uint16_t;

  protected:
    struct event_queue {
      sl::index          index = 0;
      std::vector<vbyte> events;
    };

    struct slot_info {
      std::string address = localhost;
      uint16_t    port    = any_port;
      vbyte       token;

      sl::index id_actor     = engine::id_undefined;
      bool      is_connected = true;
      bool      is_encrypted = false;
      bool      is_exclusive = false;
      bool      request_flag = true;
      sl::time  outdate      = 0;
      sl::time  wait         = 0;

      sl::vector<vbyte> in;
      sl::vector<vbyte> out;

      event_queue queue;
      transfer    tran;

      std::unique_ptr<udp_node> node;
    };

    /*  Returns false if the event should be
     *  added to the main queue.
     */
    [[nodiscard]] virtual auto perform_control(sl::index  slot,
                                               span_cbyte seq)
        -> bool;

    void cleanup();

    [[nodiscard]] auto is_encryption_enabled() const noexcept -> bool;
    [[nodiscard]] auto get_local_time() const noexcept -> sl::time;

    void update_world(sl::time delta_msec);

    void send_events();

    void send_event_to(sl::index slot, span_cbyte seq);
    void send_event(span_cbyte seq);

    void append_event(sl::index slot, span_cbyte seq);

    void set_max_slot_count(sl::whole count);
    void set_master(bool is_master);

    [[nodiscard]] auto is_allowed(sl::index slot,
                                  uint16_t  command_id) const -> bool;

    [[nodiscard]] auto is_master() const -> bool;

    auto add_slot(std::string_view address, uint16_t port)
        -> sl::index;

    [[nodiscard]] auto has_slot(std::string_view address,
                                uint16_t         port) const -> bool;

    [[nodiscard]] auto find_slot(std::string_view address,
                                 uint16_t         port) -> sl::index;

    void process_slots();
    void process_queue(sl::index slot);
    void check_outdate(sl::index slot);
    void clean_slots();
    void process_event(sl::index slot, span_cbyte seq);
    void distribute_event(sl::index slot, span_cbyte seq);
    void add_instant_event(span_cbyte seq);
    void perform_event(sl::index slot, span_cbyte seq);
    void perform_instant_events();

    void receive_events();
    void process_buffers();
    void receive_chunks();

    void receive_from(sl::index slot);
    void process_chunk(sl::index slot, span_cbyte chunk);
    void send_event_history_to(sl::index slot);

    void inc_buffer_size();

    void add_event(sl::index slot, span_cbyte seq);
    void send_chunks();
    void disconnect(sl::index slot);

    void update_slots(sl::time delta_msec);
    void update_local_time(sl::time delta_msec);
    void update_time_limit(sl::time time);

    [[nodiscard]] auto convert_delta(sl::time delta_msec) -> sl::time;
    [[nodiscard]] auto adjust_overtake(sl::time time) -> sl::time;

    std::vector<slot_info>    m_slots;
    std::unique_ptr<udp_node> m_node;

  private:
    [[nodiscard]] auto has_free_slots() const -> bool;

    vbyte       m_buffer = vbyte(default_chunk_size);
    event_queue m_queue;

    std::unique_ptr<engine::loader> m_loader;

    sl::vector<vbyte> m_instant_events;

    vuint16   m_allowed_commands;
    uint16_t  m_max_command_id        = default_max_command_id;
    bool      m_is_master             = false;
    bool      m_is_encryption_enabled = true;
    sl::whole m_max_slot_count        = 0;
    sl::whole m_loss_compensation     = default_loss_compensation;
    sl::time  m_local_time            = 0;
    sl::time  m_time_limit            = 0;
    sl::time  m_ping_clock            = 0;
  };
}

#endif
