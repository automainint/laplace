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
#include "server.h"
#include "transfer.h"
#include "udp_node.h"

namespace laplace::network {
  /*  Base class for host and
   *  remote servers.
   */
  class udp_server : public server {
  public:
    static constexpr size_t slot_host            = -1;
    static constexpr size_t slot_count_unlimited = -1;

    static constexpr size_t default_chunk_size        = 2096;
    static constexpr size_t chunk_size_increment      = 128;
    static constexpr size_t chunk_size_limit          = 0x4000;
    static constexpr size_t default_loss_compensation = 4;

    ~udp_server() override;

    void set_encryption_enabled(bool is_enabled) noexcept;
    void set_allowed_commands(cref_vuint16 commands);

    void set_chunk_size(size_t size);

    void queue(cref_vbyte seq) override;
    void tick(uint64_t delta_msec) override;

    [[nodiscard]] auto get_port() const -> uint16_t;

  protected:
    struct event_queue {
      size_t             index = 0;
      std::vector<vbyte> events;
    };

    struct slot_info {
      std::string address = localhost;
      uint16_t    port    = any_port;

      size_t   id_actor     = engine::id_undefined;
      bool     is_connected = true;
      bool     is_encrypted = false;
      bool     request_flag = true;
      uint64_t outdate      = 0;
      uint64_t wait         = 0;

      std::vector<vbyte> in;
      std::vector<vbyte> out;

      event_queue queue;
      transfer    tran;
    };

    /*  Returns false if the event should be
     *  added to the main queue.
     */
    [[nodiscard]] virtual auto perform_control( //
        size_t     slot,                        //
        cref_vbyte seq) -> bool;

    void cleanup();

    [[nodiscard]] auto is_encryption_enabled() const noexcept -> bool;
    [[nodiscard]] auto get_local_time() const noexcept -> uint64_t;

    void update_world(uint64_t delta_msec);

    void send_events();

    void send_event_to(size_t slot, cref_vbyte seq);
    void send_event(cref_vbyte seq);

    void append_event(size_t slot, cref_vbyte seq);

    void set_max_slot_count(size_t count);
    void set_master(bool is_master);

    [[nodiscard]] auto is_allowed(size_t command_id) const -> bool;

    [[nodiscard]] auto is_master() const -> bool;

    auto add_slot(std::string_view address, uint16_t port) -> size_t;

    [[nodiscard]] auto has_slot(std::string_view address,
                                uint16_t         port) const -> bool;

    [[nodiscard]] auto find_slot(std::string_view address,
                                 uint16_t         port) -> size_t;

    void process_slots();
    void process_queue(size_t slot);
    void check_outdate(size_t slot);
    void clean_slots();
    void process_event(size_t slot, cref_vbyte seq);
    void distribute_event(size_t slot, cref_vbyte seq);
    void instant_event(size_t slot, cref_vbyte seq);
    void perform_event(size_t slot, cref_vbyte seq);

    void receive_events();
    void process_buffers();
    void receive_chunks();

    void add_event(size_t slot, cref_vbyte seq);
    void send_chunks();
    void disconnect(size_t slot);

    void update_slots(uint64_t delta_msec);
    void update_local_time(uint64_t delta_msec);
    void update_time_limit(uint64_t time);

    [[nodiscard]] auto convert_delta(uint64_t delta_msec) -> uint64_t;
    [[nodiscard]] auto adjust_overtake(uint64_t time) -> uint64_t;

    std::vector<slot_info> m_slots;

    std::unique_ptr<udp_node> m_node;

  private:
    [[nodiscard]] auto has_free_slots() const -> bool;

    vbyte       m_buffer = vbyte(default_chunk_size);
    event_queue m_queue;

    vuint16  m_allowed_commands;
    bool     m_is_master             = false;
    bool     m_is_encryption_enabled = true;
    size_t   m_max_slot_count        = 0;
    size_t   m_loss_compensation     = default_loss_compensation;
    uint64_t m_local_time            = 0;
    uint64_t m_time_limit            = 0;
    uint64_t m_ping_clock            = 0;
  };
}

#endif
