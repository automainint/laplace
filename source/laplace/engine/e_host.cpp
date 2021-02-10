/*  laplace/engine/e_host.cpp
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

#include "host.h"
#include "protocol/basic_event.h"
#include "protocol/client_ping.h"
#include "protocol/ids.h"
#include "protocol/public_key.h"
#include "protocol/request_events.h"
#include "protocol/server_clock.h"
#include "protocol/server_seed.h"
#include "protocol/slot_create.h"
#include "protocol/slot_remove.h"

namespace laplace::engine {
  using namespace network;
  using namespace protocol;
  using namespace std;

  host::host() {
    setup_world();
  }

  host::~host() {
    if (m_node) {
      perform_event(slot_host, encode(server_quit()));

      send_events();
    }
  }

  void host::set_allowed_commands(cref_vuint16 commands) {
    m_allowed_commands.assign(commands.begin(), commands.end());
  }

  void host::listen(uint16_t port) {
    set_connected(true);

    m_node = make_unique<udp_node>();
    m_node->bind(port);

    perform_event(
        slot_host, encode(server_clock(get_tick_duration())));

    perform_event(slot_host,
                  encode(server_seed(get_solver()->get_seed())));
  }

  void host::queue(cref_vbyte seq) {
    if (!seq.empty()) {
      perform_event(slot_host, seq);
    } else {
      verb("[ host ] ignore empty event");
    }
  }

  void host::tick(size_t delta_msec) {
    receive_events();

    process_slots();

    send_events();

    server::tick(delta_msec);
  }

  auto host::get_port() const -> uint16_t {
    return m_node ? m_node->get_port() : any_port;
  }

  void host::receive_events() {
    receive_chunks();

    process_buffers();
  }

  void host::process_slots() {
    for (size_t i = 0; i < m_slots.size(); i++) {
      perform_queue(i);
    }

    clean_slots();
  }

  void host::send_events() {
    while (m_queue.index < m_queue.events.size()) {
      send_event(m_queue.events[m_queue.index++]);
    }

    send_chunks();
  }

  void host::receive_chunks() {
    if (m_node) {
      for (;;) {
        auto n = m_node->receive_to(
            m_buffer.data(), get_chunk_size(), async);
        if (n == 0)
          break;

        auto slot = find_slot(m_node->get_remote_address(),
                              m_node->get_remote_port());

        if (m_slots[slot].cipher.is_ready()) {
          dump({ m_buffer.data(), n });
        }

        auto plain = m_slots[slot].cipher.decrypt(
            { m_buffer.data(), n });

        if (plain.empty()) {
          verb("[ host ] unable to decrypt chunk");
          continue;
        }

        dump(plain);

        if (is_verbose()) {
          verb("[ host ] RECV %zd bytes on slot %zd",
               plain.size(), slot);
        }

        m_slots[slot].buffer.insert(m_slots[slot].buffer.end(),
                                    plain.begin(), plain.end());
      }
    }
  }

  void host::process_buffers() {
    for (size_t slot = 0; slot < m_slots.size(); slot++) {
      auto & buf   = m_slots[slot].buffer;
      size_t index = 0;

      while (buf.size() - index >= 4) {
        const size_t size = rd<uint16_t>(buf, index);

        if (buf.size() - index < size + 2) {
          verb("[ host ] ignore corrupted data");

          buf.clear();
          break;
        }

        const auto id         = rd<uint16_t>(buf, index + 2);
        bool       is_allowed = m_allowed_commands.empty();

        for (size_t i = 0; i < m_allowed_commands.size(); i++) {
          if (m_allowed_commands[i] == id) {
            is_allowed = true;
            break;
          }
        }

        if (is_allowed) {
          add_event(slot, { buf.data() + index + 2, size });
        } else {
          if (id < ids::_native_count) {
            verb("[ host ] command '%s (%hd)' not allowed",
                 ids::table[id].data(), id);
          } else {
            verb("[ host ] command '%hd' not allowed", id);
          }
        }

        index += size + 2;
      }

      if (buf.size() >= index) {
        buf.erase(buf.begin(), buf.begin() + index);
      }
    }
  }

  void host::perform_queue(size_t slot) {
    auto & qu = m_slots[slot].queue;
    size_t n  = 0;

    for (; n < qu.events.size(); n++) {
      auto &seq = qu.events[n];
      if (seq.empty())
        break;
      perform_event(slot, seq);
    }

    if (n < qu.events.size()) {
      vuint events;

      for (size_t i = n; i < qu.events.size(); i++) {
        if (qu.events[i].empty()) {
          events.emplace_back(qu.index + i);
        }

        if (events.size() >= request_events::max_event_count) {
          break;
        }
      }

      send_event_to(slot, encode(request_events(events)));
    }

    qu.events.erase(qu.events.begin(), qu.events.begin() + n);

    qu.index += n;
  }

  void host::clean_slots() {
    for (size_t i = 0; i < m_slots.size();) {
      if (m_slots[i].id_actor == id_undefined) {
        m_slots.erase(m_slots.begin() + i);
      } else {
        i++;
      }
    }
  }

  void host::send_event(cref_vbyte seq) {
    if (slot_create::scan(seq)) {
      auto buf = vbyte(seq.begin(), seq.end());

      for (size_t i = 0; i < m_slots.size(); i++) {
        slot_create::alter(buf, m_slots[i].id_actor);
        append_event(m_slots[i].chunks, buf);
      }
    } else {
      for (size_t i = 0; i < m_slots.size(); i++) {
        append_event(m_slots[i].chunks, seq);
      }
    }
  }

  void host::send_chunks() {
    for (size_t i = 0; i < m_slots.size(); i++) {
      auto &buf = m_slots[i].chunks;

      if (buf.empty())
        continue;

      auto chunk_size = adjust_chunk_size(buf);

      if (chunk_size == 0) {
        verb("[ host ] unable to adjust chunk size");

        buf.clear();
        continue;
      }

      dump({ buf.data(), chunk_size });

      if (m_slots[i].encrypt) {
        auto chunk = m_slots[i].cipher.encrypt(
            { buf.data(), chunk_size });

        if (chunk.size() == 0) {
          verb("[ host ] unable to encrypt chunk");

          buf.clear();
          continue;
        }

        dump(chunk);

        if (is_verbose()) {
          verb("[ host ] SEND %zd bytes to %s:%d (encrypted)",
               chunk.size(), m_slots[i].address.c_str(),
               static_cast<int>(m_slots[i].port));
        }

        m_node->send_to(
            m_slots[i].address, m_slots[i].port, chunk);
      } else {
        m_slots[i].encrypt = m_slots[i].cipher.is_ready();

        if (is_verbose()) {
          verb("[ host ] SEND %zd bytes to %s:%d", chunk_size,
               m_slots[i].address.c_str(),
               static_cast<int>(m_slots[i].port));
        }

        m_node->send_to(m_slots[i].address, m_slots[i].port,
                        { buf.data(), chunk_size });
      }

      buf.erase(buf.begin(), buf.begin() + chunk_size);
    }
  }

  auto host::find_slot(string_view address, uint16_t port)
      -> size_t {
    for (size_t i = 0; i < m_slots.size(); i++) {
      if (m_slots[i].address == address && m_slots[i].port == port)
        return i;
    }

    auto n = m_slots.size();

    m_slots.resize(n + 1);

    m_slots[n].address = address;
    m_slots[n].port    = port;

    return n;
  }

  void host::add_event(size_t slot, cref_vbyte seq) {
    auto  index = basic_impact::get_index(seq);
    auto &qu    = m_slots[slot].queue;

    if (index == -1) {
      /*  For public_key, request_events and client_ping.
       */
      perform_control(slot, seq);
    } else if (index >= qu.index) {
      auto n = index - qu.index;

      if (n >= qu.events.size()) {
        qu.events.resize(n + 1);
      }

      if (qu.events[n].empty()) {
        qu.events[n].assign(seq.begin(), seq.end());
      }
    } else {
      verb("[ host ] event %zd duplicate on slot %zd", index,
           slot);
    }
  }

  void host::perform_event(size_t slot, cref_vbyte seq) {
    if (basic_impact::get_time(seq) != time_undefined) {
      verb("[ host ] ignore timed event");
      return;
    }

    if (perform_control(slot, seq))
      return;

    if (auto f = get_factory(); f) {
      auto ev = f->decode(seq);

      if (!ev) {
        verb("[ host ] Unable to decode command.");
        return;
      }

      /*  Set the event actor id.
       */
      if (slot != slot_host)
        ev->set_actor(m_slots[slot].id_actor);

      /*  Set the event index.
       */
      ev->set_order({ m_queue.events.size() });

      /*  Add the event to the main queue.
       */
      m_queue.events.emplace_back(ev->encode());

      if (get_state() == server_state::prepare) {
        if (auto wor = get_world(); wor) {
          /*  Perform the event. The event
           *  time will be undefined.
           */
          ev->perform({ *wor, access::sync });
        } else {
          verb("[ host ] no world");
        }
      } else {
        if (auto sol = get_solver(); sol) {
          /*  Apply the event and
           *  set the event time.
           */
          sol->apply(ev);
        }
      }
    } else {
      verb("[ host ] no factory");
    }
  }

  void host::send_event_to(size_t slot, cref_vbyte seq) {
    if (slot_create::scan(seq)) {
      auto buf = vbyte(seq.begin(), seq.end());
      slot_create::alter(buf, m_slots[slot].id_actor);

      append_event(m_slots[slot].chunks, buf);
    } else {
      append_event(m_slots[slot].chunks, seq);
    }
  }

  auto host::perform_control(size_t slot, cref_vbyte seq)
      -> bool {
    if (client_ping::scan(seq)) {
      send_event_to(slot, seq);
      return true;
    }

    if (server_launch::scan(seq)) {
      /*  Send the event to clients.
       */
      return false;
    }

    if (server_action::scan(seq)) {
      set_state(server_state::action);
      return true;
    }

    if (server_pause::scan(seq)) {
      set_state(server_state::pause);
      return true;
    }

    if (server_clock::scan(seq)) {
      set_tick_duration(server_clock::get_tick_duration(seq));
      return false;
    }

    if (server_seed::scan(seq)) {
      if (get_state() == server_state::prepare) {
        /*  It makes no sense to change the seed
         *  after preparation.
         */
        set_random_seed(server_seed::get_seed(seq));
        return false;
      } else {
        verb("[ host ] ignore seed command");
      }

      return true;
    }

    if (server_quit::scan(seq)) {
      /*  Send the event to clients.
       */
      return false;
    }

    if (public_key::scan(seq)) {
      if (public_key::get_cipher(seq) == ids::cipher_dh_rabbit) {
        send_event_to(
            slot, encode(public_key(
                      ids::cipher_dh_rabbit,
                      m_slots[slot].cipher.get_public_key())));

        m_slots[slot].encrypt = false;

        m_slots[slot].cipher.set_remote_key(
            public_key::get_key(seq));

        if (is_verbose()) {
          verb("[ host ] mutual key");
        }

        dump(m_slots[slot].cipher.get_mutual_key());
      } else {
        verb("[ host ] unknown cipher");
      }

      return true;
    }

    if (request_events::scan(seq) && slot != slot_host) {
      const auto count = request_events::get_event_count(seq);

      for (size_t i = 0; i < count; i++) {
        auto n = request_events::get_event(seq, i);

        if (n < m_queue.events.size()) {
          send_event_to(slot, m_queue.events[n]);
        } else if (is_verbose()) {
          verb("[ host ] no requested event %zd", n);
        }
      }

      return true;
    }

    if (client_enter::scan(seq) && slot != slot_host) {
      if (auto wor = get_world(); wor) {
        m_slots[slot].id_actor = get_world()->reserve(
            id_undefined);

        perform_event(slot, encode(slot_create(false)));
      } else {
        verb("[ host ] no world");
      }

      return true;
    }

    if (client_leave::scan(seq) && slot != slot_host) {
      if (get_state() == server_state::prepare) {
        perform_event(slot, encode(slot_remove()));

        m_slots[slot].id_actor = id_undefined;
      }

      return true;
    }

    if (client_ready::scan(seq) && slot != slot_host) {
      if (get_state() == server_state::prepare) {
        if (auto f = get_factory(); f) {
          auto ev = f->decode(seq);

          if (ev) {
            if (auto wor = get_world(); wor) {
              ev->perform({ *get_world(), access::sync });
            } else {
              verb("[ host ] no world");
            }
          } else {
            verb("[ host ] unable to decode command");
          }
        } else {
          verb("[ host ] no factory");
        }
      } else {
        verb("[ host ] ignore command");
      }

      return true;
    }

    return false;
  }
}
