/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_PROTOCOL_IDS_H
#define LAPLACE_ENGINE_PROTOCOL_IDS_H

#include "../../core/static_shaffle.h"

namespace laplace::engine::protocol::ids {
  enum cipher_id : uint16_t { cipher_plain, cipher_ecc_rabbit };

  static constexpr auto _id = core::static_shaffle<0, 64>(1);

  enum command_id : uint16_t {
    undefined = _id[0],

    /*  Unindexed control commands.
     *
     *  Request to resend specified events.
     *
     *  uint16_t    id
     *  uint64_t[]  events
     */
    request_events = _id[1],

    /*  Request to send session token.
     *
     *  uint16_t    id
     */
    request_token = _id[2],

    /*  uint16_t    id
     *  uint16_t    cipher
     *  uint8_t[]   public key
     */
    session_request = _id[3],

    /*  uint16_t    id
     *  uint16_t    port
     *  uint8_t[]   public key
     */
    session_response = _id[4],

    /*  uint16_t    id
     *  uint8_t[]   token
     */
    session_token = _id[5],

    /*  uint16_t    id
     *  uint64_t    time msec
     */
    ping_request = _id[6],

    /*  uint16_t    id
     *  uint64_t    time msec
     */
    ping_response = _id[7],

    /*  uint16_t    id
     */
    client_desync = _id[8],

    /*  uint16_t    id
     *  uint64_t    last index
     *  uint64_t    time msec
     */
    server_heartbeat = _id[9],

    /*  Reserve entity ids.
     *
     *  uint16_t    id
     *  uint64_t    index
     *  uint64_t    entity count
     */
    server_reserve = _id[10],

    /*  Indexed control commands
     *
     *  Server commands.
     *
     *  uint16_t    id
     *  uint64_t    index
     */
    server_init = _id[11],

    /*  Custom loading command.
     *
     *  uint16_t    id
     *  uint64_t    index
     *  ...         data
     */
    server_loading = _id[12],

    /*  uint16_t    id
     *  uint64_t    index
     */
    server_launch = _id[13],

    /*  uint16_t    id
     *  uint64_t    index
     */
    server_action = _id[14],

    /*  uint16_t    id
     *  uint64_t    index
     */
    server_pause = _id[15],

    /*  uint16_t    id
     *  uint64_t    index
     *  uint64_t    tick duration msec
     */
    server_clock = _id[16],

    /*  uint16_t    id
     *  uint64_t    index
     *  uint32_t    seed
     */
    server_seed = _id[17],

    /*  uint16_t    id
     *  uint64_t    index
     */
    server_quit = _id[18],

    /*  Client commands.
     *
     *  uint16_t    id
     *  uint64_t    index
     */
    client_enter = _id[19],

    /*  uint16_t    id
     *  uint64_t    index
     */
    client_leave = _id[20],

    /*  uint16_t    id
     *  uint64_t    index
     */
    client_ready = _id[21],

    /*  Timed events.
     *
     *  Debug command.
     */
    debug = _id[22],

    /*  Create new slot.
     *
     *  uint16_t    id
     *  uint64_t    index
     *  uint64_t    time
     *  uint64_t    id actor
     */
    slot_create = _id[23],

    /*  Remove existing slot.
     *
     *  uint16_t    id
     *  uint64_t    index
     *  uint64_t    time
     *  uint64_t    id actor
     */
    slot_remove = _id[24],

    /*  Upper native command id value.
     */
    _native_upper = _id.upper(25)
  };
}

#endif
