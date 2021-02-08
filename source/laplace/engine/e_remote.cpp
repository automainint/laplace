/*  laplace/engine/e_remote.cpp
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

#include "protocol/basic_event.h"
#include "protocol/client_ping.h"
#include "protocol/ids.h"
#include "protocol/public_key.h"
#include "protocol/request_events.h"
#include "protocol/server_clock.h"
#include "protocol/server_seed.h"
#include "remote.h"

namespace laplace::engine {
  using namespace network;
  using namespace protocol;
  using namespace std;

  remote::~remote() {
    cleanup();
  }

  void remote::set_port(uint16_t port) {
    m_client_port = port;
  }

  void remote::connect(string_view host_address,
                       uint16_t    host_port) {
    cleanup();

    m_node = make_unique<udp_node>();
    m_node->bind(m_client_port);

    m_host_address = host_address;
    m_host_port    = host_port;

    send_event(encode(public_key(
        ids::cipher_dh_rabbit, m_cipher.get_public_key())));

    queue(encode(client_enter()));
  }

  void remote::queue(cref_vbyte seq) {
    if (!seq.empty()) {
      auto &qu = m_local_queue.events;

      qu.emplace_back(vbyte(seq.begin(), seq.end()));

      basic_impact::set_index(qu.back(), qu.size() - 1);
    }
  }

  void remote::tick(size_t delta_msec) {
    receive_events();

    perform_queue();

    send_events();

    update_world(delta_msec);

    m_time_msec += delta_msec;
  }

  auto remote::is_connected() -> bool {
    return m_node ? true : false;
  }

  auto remote::get_port() const -> uint16_t {
    return m_node ? m_node->get_port() : m_client_port;
  }

  void remote::cleanup() {
    if (m_node) {
      queue(encode(client_leave()));

      send_events();
    }

    m_local_queue.index = 0;
    m_local_queue.events.clear();
  }

  void remote::receive_events() {
    receive_chunks();

    process_buffer();
  }

  void remote::perform_queue() {
    size_t n = 0;

    for (; n < m_queue.events.size(); n++) {
      if (m_queue.events[n].empty())
        break;
      perform_event(m_queue.events[n]);
    }

    if (n < m_queue.events.size()) {
      vuint events;

      for (size_t i = n; i < m_queue.events.size(); i++) {
        if (m_queue.events[n].empty()) {
          events.emplace_back(m_queue.index + i);
        }

        if (events.size() >= request_events::max_event_count) {
          break;
        }
      }

      send_event(encode(request_events(events)));
    }

    m_queue.events.erase(
        m_queue.events.begin(), m_queue.events.begin() + n);

    m_queue.index += n;
  }

  void remote::send_events() {
    while (m_local_queue.index < m_local_queue.events.size()) {
      send_event(m_local_queue.events[m_local_queue.index++]);
    }

    send_chunk();
  }

  void remote::update_world(size_t delta_msec) {
    if (get_state() != server_state::prepare) {
      get_solver()->solve(convert_delta(delta_msec));
    }
  }

  void remote::receive_chunks() {
    if (m_node) {
      for (;;) {
        auto n = m_node->receive_to(
            m_buffer.data(), get_chunk_size(), async);
        if (n == 0)
          break;

        if (m_node->get_remote_address() != m_host_address ||
            m_node->get_remote_port() != m_host_port) {
          verb("[ client ] ignore unknown host address");
          continue;
        }

        if (is_verbose()) {
          verb("[ client ] RECV %zd bytes", n);
        }

        if (m_cipher.is_ready()) {
          dump({ m_buffer.data(), n });
        }

        auto plain = m_cipher.decrypt({ m_buffer.data(), n });

        if (!plain.empty()) {
          dump(plain);

          m_client_buffer.insert(m_client_buffer.end(),
                                 plain.begin(), plain.end());
        } else {
          verb("[ client ] unable to decrypt chunk");
        }
      }
    }
  }

  void remote::process_buffer() {
    size_t index = 0;

    while (m_client_buffer.size() - index >= 4) {
      size_t size = rd<uint16_t>(m_client_buffer, index);

      if (m_client_buffer.size() - index < size + 2) {
        verb("[ client ] ignore corrupted data");

        m_client_buffer.clear();
        break;
      }

      add_event({ m_client_buffer.data() + index + 2, size });

      index += size + 2;
    }

    if (m_client_buffer.size() >= index) {
      m_client_buffer.erase(m_client_buffer.begin(),
                            m_client_buffer.begin() + index);
    }
  }

  void remote::perform_event(cref_vbyte seq) {
    if (basic_impact::get_time(seq) == time_undefined) {
      if (perform_control(seq))
        return;

      if (auto f = get_factory(); f) {
        auto ev = f->decode(seq);

        if (!ev) {
          verb("[ client ] unable to decode command");
          return;
        }

        ev->perform({ *get_world(), access::sync });
      }
    } else {
      if (auto f = get_factory(); f) {
        auto ev = f->decode(seq);

        if (!ev)
          return;

        if (m_time_limit < ev->get_time())
          m_time_limit = ev->get_time();

        get_solver()->apply(ev);
      }
    }
  }

  auto remote::convert_delta(size_t delta_msec) -> uint64_t {
    auto time = get_solver()->get_time();

    auto delta = adjust_delta(delta_msec) + adjust_overtake(time);

    return min(delta, m_time_limit - time);
  }

  void remote::send_event(cref_vbyte seq) {
    append_event(m_chunks, seq);
  }

  void remote::send_chunk() {
    if (!m_chunks.empty()) {
      size_t chunk_size = adjust_chunk_size(m_chunks);

      if (chunk_size == 0) {
        verb("[ client ] unable to adjust chunk size");

        m_chunks.clear();
        return;
      }

      if (m_cipher.is_ready()) {
        dump({ m_chunks.data(), chunk_size });
      }

      auto chunk = m_cipher.encrypt(
          { m_chunks.data(), chunk_size });

      if (chunk.size() == 0) {
        verb("[ client ] unable to encrypt chunk");

        m_chunks.clear();
        return;
      }

        dump(chunk);

      if (is_verbose()) {
        verb("[ client ] SEND %zd bytes to %s:%d%s",
             chunk.size(), m_host_address.c_str(),
             static_cast<int>(m_host_port),
             m_cipher.is_ready() ? " (encrypted)" : "");
      }

      m_node->send_to(m_host_address, m_host_port, chunk);

      m_chunks.erase(
          m_chunks.begin(), m_chunks.begin() + chunk_size);
    }
  }

  void remote::add_event(cref_vbyte seq) {
    auto index = basic_impact::get_index(seq);

    if (index == -1) {
      /*  For public_key, request_events and client_ping.
       */
      if (!perform_control(seq)) {
        verb("[ client ] ignore unindexed command");
      }

    } else if (index >= m_queue.index) {
      auto n = index - m_queue.index;

      if (n >= m_queue.events.size()) {
        m_queue.events.resize(n + 1);
      }

      if (m_queue.events[n].empty()) {
        m_queue.events[n].assign(seq.begin(), seq.end());
      } else {
        verb("[ client ] command already written");
      }
    } else {
      verb("[ client ] ignore command duplicate");
    }
  }

  auto remote::perform_control(cref_vbyte seq) -> bool {
    if (public_key::scan(seq)) {
      if (public_key::get_cipher(seq) == ids::cipher_dh_rabbit) {
        m_cipher.set_remote_key(public_key::get_key(seq));

        if (is_verbose()) {
          verb("[ client ] mutual key");
        }

        dump(m_cipher.get_mutual_key());
      }

      return true;
    }

    if (request_events::scan(seq)) {
      const auto count = request_events::get_event_count(seq);

      for (size_t i = 0; i < count; i++) {
        auto n = request_events::get_event(seq, i);

        if (n < m_local_queue.events.size()) {
          send_event(m_local_queue.events[n]);
        }
      }

      return true;
    }

    if (server_launch::scan(seq)) {
      /*  Perform event.
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
      return true;
    }

    if (server_seed::scan(seq)) {
      const auto seed = server_seed::get_seed(seq);

      if (get_state() == server_state::prepare) {
        set_random_seed(seed);
      }

      return true;
    }

    if (server_quit::scan(seq)) {
      m_node.reset();
      return true;
    }

    if (client_ping::scan(seq)) {
      const auto ping = m_time_msec -
                        client_ping::get_ping_time(seq);

      set_ping(ping);
      return true;
    }

    return false;
  }

  auto remote::adjust_overtake(uint64_t time) -> uint64_t {
    uint64_t overtake = 0;

    if (get_tick_duration() > 0) {
      auto ping_ticks = get_ping() / get_tick_duration();
      auto time_gape  = m_time_limit - time;

      if (time_gape > ping_ticks)
        overtake = time_gape - ping_ticks;
    }

    return overtake;
  }
}
