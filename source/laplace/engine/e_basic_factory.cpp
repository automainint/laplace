/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "basic_factory.h"

#include "../core/parser.h"
#include "protocol/all.h"

namespace laplace::engine {
  using namespace protocol;

  using std::string_view, std::string, std::span, std::u8string,
      core::parser;

  auto basic_factory::parse(string_view command) const -> vbyte {
    return parse_native(id_by_name_native, command);
  }

  auto basic_factory::print(span_cbyte seq) const -> string {
    return print_native(name_by_id_native, seq);
  }

  auto basic_factory::decode(span_cbyte seq) const
      -> ptr_prime_impact {
    return decode_native(seq);
  }

  auto basic_factory::parse_multi(std::string_view commands) const
      -> sl::vector<vbyte> {

    constexpr sl::whole avg_command_size = 20;

    auto result = sl::vector<vbyte> {};
    auto n      = sl::index {};

    result.reserve(commands.size() / avg_command_size);

    for (sl::index i = 0; i < commands.size(); i++) {
      if (commands[i] == '\n') {
        result.emplace_back(
            parse({ commands.data() + n,
                    static_cast<string_view::size_type>(i - n) }));

        n = i + 1;
      }
    }

    return result;
  }

  auto basic_factory::print_multi(span<const span_cbyte> seqs) const
      -> string {

    auto cmds = string {};

    for (auto &seq : seqs) {
      cmds.append(print(seq));
      cmds.append(1, '\n');
    }

    return cmds;
  }

  auto basic_factory::id_by_name_native(string_view name)
      -> uint16_t {
    if (name == "request-events")
      return ids::request_events;
    if (name == "request-token")
      return ids::request_token;
    if (name == "session-request")
      return ids::session_request;
    if (name == "session-response")
      return ids::session_response;
    if (name == "session-token")
      return ids::session_token;
    if (name == "ping-request")
      return ids::ping_request;
    if (name == "ping-response")
      return ids::ping_response;
    if (name == "client-desync")
      return ids::client_desync;
    if (name == "server-idle")
      return ids::server_idle;
    if (name == "server-init")
      return ids::server_init;
    if (name == "server-loading")
      return ids::server_loading;
    if (name == "server-launch")
      return ids::server_launch;
    if (name == "server-action")
      return ids::server_action;
    if (name == "server-pause")
      return ids::server_pause;
    if (name == "server-reserve")
      return ids::server_reserve;
    if (name == "server-clock")
      return ids::server_clock;
    if (name == "server-seed")
      return ids::server_seed;
    if (name == "server-quit")
      return ids::server_quit;
    if (name == "client-enter")
      return ids::client_enter;
    if (name == "client-leave")
      return ids::client_leave;
    if (name == "client-ready")
      return ids::client_ready;
    if (name == "debug")
      return ids::debug;
    if (name == "slot-create")
      return ids::slot_create;
    if (name == "slot-remove")
      return ids::slot_remove;

    return ids::undefined;
  }

  auto basic_factory::name_by_id_native(uint16_t id) -> string {
    if (id == ids::request_events)
      return string("request-events");
    if (id == ids::request_token)
      return string("request-token");
    if (id == ids::session_request)
      return string("session-request");
    if (id == ids::session_response)
      return string("session-response");
    if (id == ids::session_token)
      return string("session-token");
    if (id == ids::ping_request)
      return string("ping-request");
    if (id == ids::ping_response)
      return string("ping-response");
    if (id == ids::client_desync)
      return string("client-desync");
    if (id == ids::server_idle)
      return string("server-idle");
    if (id == ids::server_init)
      return string("server-init");
    if (id == ids::server_loading)
      return string("server-loading");
    if (id == ids::server_launch)
      return string("server-launch");
    if (id == ids::server_action)
      return string("server-action");
    if (id == ids::server_pause)
      return string("server-pause");
    if (id == ids::server_reserve)
      return string("server-reserve");
    if (id == ids::server_clock)
      return string("server-clock");
    if (id == ids::server_seed)
      return string("server-seed");
    if (id == ids::server_quit)
      return string("server-quit");
    if (id == ids::client_enter)
      return string("client-enter");
    if (id == ids::client_leave)
      return string("client-leave");
    if (id == ids::client_ready)
      return string("client-ready");
    if (id == ids::debug)
      return string("debug");
    if (id == ids::slot_create)
      return string("slot-create");
    if (id == ids::slot_remove)
      return string("slot-remove");

    return {};
  }

  auto basic_factory::parse_native(fn_id_by_name const &id_by_name,
                                   string_view command) -> vbyte {

    if (!id_by_name) {
      error_("No id-by-name function.", __FUNCTION__);
      return {};
    }

    auto in = parser::wrap(command);

    auto u8_id = u8string {};

    auto lin = in.get_line();
    auto col = in.get_column();

    if (!in.parse(" %s ", &u8_id)) {
      error_(fmt("Invalid syntax. Id expected (%line %dz, col %dz).",
                 lin, col),
             __FUNCTION__);
      return {};
    }

    auto const id = id_by_name(to_string(u8_id));

    if (id == ids::undefined) {
      error_(fmt("Unknown command (line %dz, col %dz).", lin, col),
             __FUNCTION__);
      return {};
    }

    auto seq = vbyte(2);
    serial::wr<uint16_t>(seq, 0, id);

    auto x = uint64_t {};

    lin = in.get_line();
    col = in.get_column();

    while (in.parse(" %X ", &x)) {
      if (x != (x & 0xff)) {
        error_(
            fmt("Invalid syntax. Byte expected (line %dz, col %dz).",
                lin, col),
            __FUNCTION__);
        return {};
      }

      seq.emplace_back(static_cast<uint8_t>(x));
    }

    if (!in.is_eof()) {
      error_(fmt("Invalid syntax. EOF expected (line %dz, col %dz).",
                 in.get_line(), in.get_column()),
             __FUNCTION__);
      return {};
    }

    return seq;
  }

  auto basic_factory::print_native(fn_name_by_id const &name_by_id,
                                   span_cbyte seq) -> string {

    if (!name_by_id) {
      error_("No name-by-id function.", __FUNCTION__);
      return {};
    }

    constexpr sl::whole max_id_size = 14;

    auto s = string {};
    s.reserve(max_id_size + seq.size() * 3);

    if (seq.size() < 2) {
      if (!seq.empty())
        error_("Invalid data.", __FUNCTION__);
      return {};
    }

    const auto id = prime_impact::get_id(seq);

    s = name_by_id(id);

    if (s.empty()) {
      error_("Invalid id.", __FUNCTION__);
      return {};
    }

    for (sl::index i = 2; i < seq.size(); i++)
      s.append(fmt(" %02x", seq[i]));

    return s;
  }

  auto basic_factory::decode_native(span_cbyte seq)
      -> ptr_prime_impact {

    if (request_events::scan(seq))
      return make<request_events>(seq);
    if (request_token::scan(seq))
      return make<request_token>(seq);
    if (session_request::scan(seq))
      return make<session_request>(seq);
    if (session_response::scan(seq))
      return make<session_response>(seq);
    if (session_token::scan(seq))
      return make<session_token>(seq);
    if (ping_request::scan(seq))
      return make<ping_request>(seq);
    if (ping_response::scan(seq))
      return make<ping_response>(seq);
    if (client_desync::scan(seq))
      return make<client_desync>(seq);
    if (server_idle::scan(seq))
      return make<server_idle>(seq);
    if (server_init::scan(seq))
      return make<server_init>(seq);
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
