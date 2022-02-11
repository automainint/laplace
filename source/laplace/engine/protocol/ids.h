/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_engine_protocol_ids_h
#define laplace_engine_protocol_ids_h

#include <cstdint>
#include <string>

namespace laplace::engine::protocol::ids {
  enum cipher_id : uint16_t { cipher_plain, cipher_ecc_rabbit };

  enum command_id : uint16_t {
    undefined = 0,

    /*  Unindexed control commands.
     *
     *  Request to resend specified events.
     *
     *  uint16_t    id
     *  uint64_t[]  events
     */
    request_events,

    /*  Request to send session token.
     *
     *  uint16_t    id
     */
    request_token,

    /*  uint16_t    id
     *  uint16_t    cipher
     *  uint8_t[]   public key
     */
    session_request,

    /*  uint16_t    id
     *  uint16_t    port
     *  uint8_t[]   public key
     */
    session_response,

    /*  uint16_t    id
     *  uint8_t[]   token
     */
    session_token,

    /*  uint16_t    id
     *  uint64_t    time msec
     */
    ping_request,

    /*  uint16_t    id
     *  uint64_t    time msec
     */
    ping_response,

    /*  uint16_t    id
     */
    // client_desync,

    /*  uint16_t    id
     *  uint64_t    last index
     *  uint64_t    time msec
     */
    server_idle,

    /*  Indexed control commands
     *
     *  Server commands.
     *
     *  uint16_t    id
     *  uint64_t    index
     */
    server_init,

    /*  Custom loading command.
     *
     *  uint16_t    id
     *  uint64_t    index
     *  ...         data
     */
    server_loading,

    /*  uint16_t    id
     *  uint64_t    index
     */
    server_launch,

    /*  uint16_t    id
     *  uint64_t    index
     */
    server_action,

    /*  uint16_t    id
     *  uint64_t    index
     */
    server_pause,

    /*  Reserve entity ids.
     *
     *  uint16_t    id
     *  uint64_t    index
     *  uint64_t    entity count
     */
    server_reserve,

    /*  uint16_t    id
     *  uint64_t    index
     *  uint64_t    tick duration msec
     */
    server_clock,

    /*  uint16_t    id
     *  uint64_t    index
     *  uint32_t    seed
     */
    server_seed,

    /*  uint16_t    id
     *  uint64_t    index
     */
    server_quit,

    /*  Client commands.
     *
     *  uint16_t    id
     *  uint64_t    index
     */
    client_enter,

    /*  uint16_t    id
     *  uint64_t    index
     */
    client_leave,

    /*  uint16_t    id
     *  uint64_t    index
     */
    client_ready,

    /*  Timed events.
     *
     *  Debug command.
     */
    debug,

    /*  Create new slot.
     *
     *  uint16_t    id
     *  uint64_t    index
     *  uint64_t    time
     *  uint64_t    id actor
     */
    slot_create,

    /*  Remove existing slot.
     *
     *  uint16_t    id
     *  uint64_t    index
     *  uint64_t    time
     *  uint64_t    id actor
     */
    slot_remove,

    /*  Total native command count.
     */
    _native_count
  };
}

#endif
