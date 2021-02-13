/*  laplace/engine/e_basic_factory.cpp
 *
 *      Basic factory implementation.
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

#include "../core/parser.h"
#include "../core/utils.h"
#include "basic_factory.h"
#include "protocol/basic_event.h"
#include "protocol/client_ping.h"
#include "protocol/debug.h"
#include "protocol/ids.h"
#include "protocol/public_key.h"
#include "protocol/request_events.h"
#include "protocol/server_clock.h"
#include "protocol/server_idle.h"
#include "protocol/server_seed.h"
#include "protocol/slot_create.h"
#include "protocol/slot_remove.h"

namespace laplace::engine {
  using namespace protocol;

  using std::string_view, std::string, std::vector, std::span,
      std::u8string, core::parser;

  auto basic_factory::parse(string_view command) const -> vbyte {
    return parse_native(
        { ids::table, ids::_native_count }, command);
  }

  auto basic_factory::print(cref_vbyte seq) const -> string {
    return print_native({ ids::table, ids::_native_count }, seq);
  }

  auto basic_factory::decode(cref_vbyte seq) const
      -> ptr_prime_impact {
    return decode_native(seq);
  }

  auto basic_factory::parse_multi(std::string_view commands) const
      -> vector<vbyte> {
    constexpr size_t avg_command_size = 20;

    vector<vbyte> result;
    size_t        index = 0;

    result.reserve(commands.size() / avg_command_size);

    for (size_t i = 0; i < commands.size(); i++) {
      if (commands[i] == '\n') {
        result.emplace_back(
            parse({ commands.data() + index, i - index }));

        index = i + 1;
      }
    }

    return result;
  }

  auto basic_factory::print_multi(span<const cref_vbyte> seqs) const
      -> string {
    string cmds;

    for (auto &seq : seqs) {
      cmds.append(print(seq));
      cmds.append(1, '\n');
    }

    return cmds;
  }

  auto basic_factory::parse_native(
      std::span<const std::string_view> table,
      string_view                       command) -> vbyte {
    auto in = parser::wrap(command);

    u8string u8_id;

    auto lin = in.get_line();
    auto col = in.get_column();

    if (!in.parse(" %a ", &u8_id)) {
      error(__FUNCTION__,
            "Invalid syntax. Id expected (%line %dz, col %dz).",
            lin, col);
      return {};
    }

    uint16_t id   = ids::undefined;
    string   s_id = to_string(u8_id);

    for (size_t i = 0; i < table.size(); i++) {
      if (table[i] == s_id) {
        id = static_cast<uint16_t>(i);
        break;
      }
    }

    if (id == ids::undefined) {
      error(__FUNCTION__,
            "Unknown command (line %dz, col %dz).", lin, col);
      return {};
    }

    vbyte seq(2);
    wr<uint16_t>(seq, 0, id);

    uint64_t x;

    lin = in.get_line();
    col = in.get_column();

    while (in.parse(" %X ", &x)) {
      if (x != (x & 0xff)) {
        error(__FUNCTION__, "Invalid syntax. Byte expected (line %dz, col %dz).",
              lin, col);
        return {};
      }

      seq.emplace_back(static_cast<uint8_t>(x));
    }

    if (!in.is_eof()) {
      error(__FUNCTION__,
            "Invalid syntax. EOF expected (line %dz, col %dz).",
            in.get_line(), in.get_column());
      return {};
    }

    return seq;
  }

  auto basic_factory::print_native(
      std::span<const std::string_view> table, cref_vbyte seq)
      -> string {
    constexpr size_t max_id_size = 14;
    string           s;
    s.reserve(max_id_size + seq.size() * 3);

    if (seq.size() >= 2) {
      auto id = prime_impact::get_id_unsafe(seq);

      if (id < table.size()) {
        s.append(table[id]);

        for (size_t i = 2; i < seq.size(); i++) {
          s.append(to_string(" %02x", seq[i]));
        }
      }
    }

    return s;
  }

  auto basic_factory::decode_native(cref_vbyte seq)
      -> ptr_prime_impact {

    if (public_key::scan(seq))
      return make<public_key>(seq);
    if (request_events::scan(seq))
      return make<request_events>(seq);
    if (server_idle::scan(seq))
      return make<server_idle>(seq);
    if (server_launch::scan(seq))
      return make<server_launch>(seq);
    if (server_action::scan(seq))
      return make<server_action>(seq);
    if (server_pause::scan(seq))
      return make<server_pause>(seq);
    if (server_clock::scan(seq))
      return make<server_clock>(seq);
    if (server_seed::scan(seq))
      return make<server_seed>(seq);
    if (server_quit::scan(seq))
      return make<server_quit>(seq);
    if (client_ping::scan(seq))
      return make<client_ping>(seq);
    if (client_enter::scan(seq))
      return make<client_enter>(seq);
    if (client_leave::scan(seq))
      return make<client_leave>(seq);
    if (client_ready::scan(seq))
      return make<client_ready>(seq);
    if (debug::scan(seq))
      return make<debug>(seq);
    if (slot_create::scan(seq))
      return make<slot_create>(seq);
    if (slot_remove::scan(seq))
      return make<slot_remove>(seq);

    return {};
  }
}
