/*  laplace/engine/protocol/ids.h
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

#ifndef laplace_engine_protocol_ids_h
#define laplace_engine_protocol_ids_h

#include <cstdint>
#include <string>

namespace laplace::engine::protocol::ids {
  constexpr std::string_view table[] = { "",
                                         "request-events",
                                         "request-token",
                                         "session-request",
                                         "session-response",
                                         "session-token",
                                         "ping-request",
                                         "ping-response",
                                         "client-desync",
                                         "server-idle",
                                         "server-init",
                                         "server-loading",
                                         "server-launch",
                                         "server-action",
                                         "server-pause",
                                         "server-clock",
                                         "server-seed",
                                         "server-quit",
                                         "client-enter",
                                         "client-leave",
                                         "client-ready",
                                         "debug",
                                         "slot-create",
                                         "slot-remove" };

  enum cipher_id : uint16_t { cipher_plain, cipher_ecc_rabbit };

  enum command_id : uint16_t {
    undefined = 0,

    /*  Request to resend specified events.
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
    client_desync,

    /*  uint16_t    id
     *  uint64_t    last index
     *  uint64_t    time msec
     */
    server_idle,

    /*  Unindexed command count.
     */
    _unindexed_count,

    /*  Server commands.
     *
     *  uint16_t    id
     *  uint64_t    index
     */
    server_init = _unindexed_count,

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

    /*  Control command count.
     */
    _control_count,

    /*  Debug command.
     */
    debug = _control_count,

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

    /*  Total command count.
     */
    _native_count
  };

  static_assert((sizeof table / sizeof *table) == _native_count);
}

#endif
