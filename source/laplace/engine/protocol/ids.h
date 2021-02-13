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

#ifndef __laplace__engine_protocol_ids__
#define __laplace__engine_protocol_ids__

#include <cstdint>
#include <string>

namespace laplace::engine::protocol::ids {
  constexpr std::string_view table[] = { //
    "",
    "publickey",
    "requestevents",
    "clientping",
    "serveridle",
    "serverlaunch",
    "serveraction",
    "serverpause",
    "serverclock",
    "serverseed",
    "serverquit",
    "cliententer",
    "clientleave",
    "clientready",
    "debug",
    "slotcreate",
    "slotremove"
  };

  enum cipher_id : uint16_t { //
    cipher_plain,             //
    cipher_dh_rabbit
  };

  enum command_id : uint16_t {
    undefined = 0,

    /*  Provide public key.
     *
     *  uint16_t    id
     *  uint16_t    cipher
     *  uint8_t     key[ ]
     */
    public_key,

    /*  Request to resend specified events.
     *
     *  uint16_t    id
     *  uint64_t    events[ ]
     */
    request_events,

    /*  uint16_t    id
     *  uint64_t    time msec
     */
    client_ping,

    /*  uint16_t    id
     *  uint64_t    time msec
     */
    server_idle,

    /*  Unindexed command count.
     */
    _unindexed_count,

    /*  Server commands.
     */

    /*  uint16_t    id
     *  uint64_t    index
     */
    server_launch = _unindexed_count,

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
     */

    /*  uint16_t    id
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
}

#endif
