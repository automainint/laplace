/*  laplace/unittests/e_protocol.test.cpp

 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/engine/protocol/slot_create.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using engine::encode, engine::prime_impact,
      engine::protocol::slot_create;

  TEST(engine, protocol_slot_create) {
    constexpr size_t   index    = 1;
    constexpr uint64_t time     = 45;
    constexpr size_t   actor    = 5;
    constexpr bool     is_local = true;

    auto seq = encode<slot_create>(index, time, actor, is_local);

    EXPECT_EQ(prime_impact::get_index(seq), index);
    EXPECT_EQ(prime_impact::get_time(seq), time);
    EXPECT_EQ(prime_impact::get_actor(seq), actor);
    EXPECT_EQ(slot_create::get_local_flag(seq), is_local);

    seq = slot_create(index, time, actor, is_local).encode();

    EXPECT_EQ(prime_impact::get_index(seq), index);
    EXPECT_EQ(prime_impact::get_time(seq), time);
    EXPECT_EQ(prime_impact::get_actor(seq), actor);
    EXPECT_EQ(slot_create::get_local_flag(seq), is_local);

    auto ev = slot_create::decode(seq);

    EXPECT_EQ(ev.get_index64(), index);
    EXPECT_EQ(ev.get_time64(), time);
    EXPECT_EQ(ev.get_actor64(), actor);
    EXPECT_EQ(ev.is_local(), is_local != 0);
  }
}
