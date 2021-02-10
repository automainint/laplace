/*  laplace/engine/host.h
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

#ifndef __laplace__engine_host__
#define __laplace__engine_host__

#include "dh_rabbit.h"
#include "udp_server.h"

namespace laplace::engine {
  /*  Host server.
   */
  class host final : public udp_server {
  public:
    static constexpr size_t slot_host = -1;

    host();
    ~host() final;

    void set_allowed_commands(cref_vuint16 commands);

    void listen(uint16_t port = network::any_port);

    void queue(cref_vbyte seq) final;
    void tick(size_t delta_msec) final;

    auto get_port() const -> uint16_t;

    template <typename impact_, typename... args_>
    inline void queue(args_... args) {
      this->queue(encode(impact_(args...)));
    }

  private:
    void receive_events();
    void process_slots();
    void send_events();

    void receive_chunks();
    void process_buffers();
    void perform_queue(size_t slot);
    void clean_slots();
    void send_event(cref_vbyte seq);
    void send_chunks();

    auto find_slot(std::string_view address, uint16_t port)
        -> size_t;
    void add_event(size_t slot, cref_vbyte seq);
    void perform_event(size_t slot, cref_vbyte seq);
    void send_event_to(size_t slot, cref_vbyte seq);

    /*  Returns false if the event should be
     *  added to the main queue.
     *
     *  Events from the main queue will be
     *  transferred to all the clients.
     */
    auto perform_control(size_t slot, cref_vbyte seq) -> bool;

    struct slot_info {
      std::string address;
      uint16_t    port     = network::any_port;
      size_t      id_actor = id_undefined;
      bool        encrypt  = false;

      dh_rabbit   cipher;
      vbyte       buffer;
      vbyte       chunks;
      event_queue queue;
    };

    vuint16 m_allowed_commands;

    std::vector<slot_info> m_slots;
    event_queue            m_queue;

    std::unique_ptr<network::udp_node> m_node;
  };
}

#endif
