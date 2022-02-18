/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "../../../laplace/core/serial.h"
#include "../../../laplace/network/crypto/ecc_rabbit.h"
#include "../../../laplace/network/pipe.h"
#include "../../../laplace/network/server.h"
#include "helpers.test.h"
#include <gtest/gtest.h>

namespace laplace::test {
  using network::server, network::pipe, std::make_shared,
      network::blank_protocol_interface, network::control,
      network::cipher, network::transfer, network::crypto::ecc_rabbit;

  TEST(network, session_request_session_response) {
    /*  Peer answers to session_request with session_response.
     *
     *    Bob                 Alice
     *  session_request   ->
     *                    <-  session_response
     */

    uint8_t const id_session_request  = 1;
    uint8_t const id_session_response = 2;

    int get_control_id_called          = 0;
    int decode_cipher_id_called        = 0;
    int decode_public_key_called       = 0;
    int encode_session_response_called = 0;

    auto proto       = blank_protocol_interface();
    proto.get_control_id = [&](span_cbyte seq) -> control {
      get_control_id_called++;
      if (serial::rd<decltype(id_session_request)>(seq, 0) ==
          id_session_request)
        return control::session_request;
      return control::undefined;
    };
    proto.decode_cipher_id = [&](span_cbyte seq) -> cipher {
      decode_cipher_id_called++;
      return cipher::rabbit;
    };
    proto.decode_public_key = [&](span_cbyte seq) -> span_cbyte {
      decode_public_key_called++;
      return { seq.begin() + sizeof id_session_request, seq.end() };
    };
    proto.encode_session_response = [&](uint16_t   port,
                                        span_cbyte key) -> vbyte {
      encode_session_response_called++;
      return serial::pack_to_bytes(id_session_response, port, key);
    };

    auto io = make_shared<pipe>();

    auto alice = server {};
    alice.setup_protocol(proto);
    alice.enable_encryption(true);

    auto session = alice.await_listen({ .io = io, .port = 1 });

    auto bob  = io->open(2);
    auto tran = transfer {};
    tran.setup_cipher<ecc_rabbit>();

    EXPECT_TRUE(_send(bob, alice.get_port(), tran, id_session_request,
                      tran.get_public_key()));

    session.resume();

    auto const received = _receive(bob, tran);

    EXPECT_EQ(received.size(), 1);
    if (!received.empty())
      EXPECT_TRUE(serial::rd<decltype(id_session_response)>(
                      received[0], 0) == id_session_response);

    EXPECT_EQ(get_control_id_called, 1);
    EXPECT_EQ(decode_cipher_id_called, 1);
    EXPECT_EQ(decode_public_key_called, 1);
    EXPECT_EQ(encode_session_response_called, 1);
  }
}
