/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/core/serial.h"
#include "../../laplace/engine/basic_factory.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::basic_factory, std::string_view;
  using namespace engine::protocol;

  TEST(engine, basic_factory_id_name) {
    auto fac = basic_factory {};

    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::request_events)),
              ids::request_events);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::request_token)),
              ids::request_token);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::session_request)),
              ids::session_request);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::session_response)),
              ids::session_response);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::session_token)),
              ids::session_token);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::ping_request)),
              ids::ping_request);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::ping_response)),
              ids::ping_response);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::client_desync)),
              ids::client_desync);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::server_heartbeat)),
              ids::server_heartbeat);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::server_init)),
              ids::server_init);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::server_loading)),
              ids::server_loading);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::server_launch)),
              ids::server_launch);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::server_action)),
              ids::server_action);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::server_pause)),
              ids::server_pause);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::server_reserve)),
              ids::server_reserve);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::server_clock)),
              ids::server_clock);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::server_clock)),
              ids::server_clock);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::server_seed)),
              ids::server_seed);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::server_quit)),
              ids::server_quit);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::client_enter)),
              ids::client_enter);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::client_leave)),
              ids::client_leave);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::client_ready)),
              ids::client_ready);
    EXPECT_EQ(
        fac.id_by_name_native(fac.name_by_id_native(ids::debug)),
        ids::debug);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::slot_create)),
              ids::slot_create);
    EXPECT_EQ(fac.id_by_name_native(
                  fac.name_by_id_native(ids::slot_remove)),
              ids::slot_remove);
    EXPECT_EQ(
        fac.id_by_name_native(fac.name_by_id_native(ids::undefined)),
        ids::undefined);
  }

  TEST(engine, basic_factory_parse) {
    auto const fac = basic_factory {};

    EXPECT_TRUE(fac.parse_native({}, "", get_global_log()).empty());
    EXPECT_TRUE(fac.parse(" ").empty());
    EXPECT_TRUE(fac.parse("__undefined__").empty());
    EXPECT_TRUE(fac.parse("session-request 99999").empty());
    EXPECT_TRUE(fac.parse("session-request 10 xyz").empty());

    auto foo = vbyte { 0, 0, 0x10, 0x20, 0x30 };
    serial::wr<uint16_t>(foo, 0, ids::session_request);

    EXPECT_EQ(fac.parse("session-request 10 20 30"), foo);
  }

  TEST(engine, basic_factory_print_1) {
    auto const fac = basic_factory {};

    EXPECT_TRUE(fac.print_native({}, {}, get_global_log()).empty());
    EXPECT_TRUE(fac.print({}).empty());
  }

  TEST(engine, basic_factory_print_2) {
    auto const fac = basic_factory {};
    auto       seq = vbyte(1);

    EXPECT_TRUE(fac.print(seq).empty());
  }

  TEST(engine, basic_factory_print_3) {
    auto const fac = basic_factory {};

    auto seq = vbyte(2);
    serial::wr<uint16_t>(seq, 0, 9999);

    EXPECT_TRUE(fac.print(seq).empty());
  }

  TEST(engine, basic_factory_print_4) {
    auto const fac = basic_factory {};

    auto seq = vbyte(2);
    serial::wr<uint16_t>(seq, 0, ids::session_request);

    EXPECT_EQ(fac.print(seq), "session-request");
  }

  TEST(engine, basic_factory_print_5) {
    auto const fac = basic_factory {};

    auto seq = vbyte { 0, 0, 0x10, 0x20, 0x30 };
    serial::wr<uint16_t>(seq, 0, ids::session_request);

    EXPECT_EQ(fac.print(seq), "session-request 10 20 30");
  }

  TEST(engine, basic_factory_multi) {
    auto const fac = basic_factory {};
    auto       foo =
        "session-request 10 20 30\nsession-token 1f ab ff 10 00\n";

    auto parsed = fac.parse_multi(foo);
    auto spans  = sl::vector<span_cbyte> {};

    for (auto &x : parsed) spans.emplace_back(x);

    EXPECT_EQ(fac.print_multi(spans), foo);
  }
}
