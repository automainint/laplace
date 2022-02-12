/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../laplace/network/crypto/ecc_rabbit.h"
#include "../../laplace/network/pipe.h"
#include "../../laplace/network/server.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using network::server, network::pipe, std::make_shared,
      network::blank_protocol_interface, network::control,
      network::cipher, network::transfer, network::crypto::ecc_rabbit;

  TEST(network, server_session_request) {
    int is_allowed_called              = 0;
    int get_control_id_called          = 0;
    int decode_cipher_id_called        = 0;
    int decode_public_key_called       = 0;
    int encode_session_response_called = 0;

    auto proto       = blank_protocol_interface();
    proto.is_allowed = [&](span_cbyte seq, bool is_exclusive) {
      is_allowed_called++;
      return !seq.empty() && seq[0] == 1;
    };
    proto.get_control_id = [&](span_cbyte seq) -> control {
      get_control_id_called++;
      if (!seq.empty())
        switch (seq[0])
        case 1:
          return control::session_request;
      return control::undefined;
    };
    proto.decode_cipher_id = [&](span_cbyte seq) -> cipher {
      decode_cipher_id_called++;
      return cipher::rabbit;
    };
    proto.decode_public_key = [&](span_cbyte seq) -> span_cbyte {
      decode_public_key_called++;
      return { seq.begin() + 1, seq.end() };
    };
    proto.encode_session_response = [&](uint16_t   port,
                                        span_cbyte key) {
      encode_session_response_called++;
      auto v = vbyte {};
      v.emplace_back(2);
      v.emplace_back(port & 255u);
      v.emplace_back((port >> 8u) & 255u);
      v.insert(v.end(), key.begin(), key.end());
      return v;
    };

    auto io = make_shared<pipe>();

    auto alice = server {};
    alice.setup_protocol(proto);
    alice.enable_encryption(true);
    alice.listen({ .io = io, .port = 1 });

    auto bob = io->open(2);

    auto tran = transfer {};
    tran.setup_cipher<ecc_rabbit>();
    auto key = tran.get_public_key();

    auto req = vbyte {};
    req.emplace_back(1);
    req.insert(req.end(), key.begin(), key.end());

    auto reqs = sl::vector<span_cbyte> { span_cbyte { req.begin(),
                                                      req.end() } };

    std::ignore = bob->send("", alice.get_port(), tran.encode(reqs));

    alice.tick(1);

    uint8_t    buf[512] = {};
    auto const n        = bob->receive(buf);

    EXPECT_EQ(is_allowed_called, 1);
    EXPECT_EQ(get_control_id_called, 1);
    EXPECT_EQ(decode_cipher_id_called, 1);
    EXPECT_EQ(decode_public_key_called, 1);
    EXPECT_EQ(encode_session_response_called, 1);

    EXPECT_TRUE(n > 3 && buf[0] == 2);
  }
}
