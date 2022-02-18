/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_INTERFACE_EXECUTION_H
#define LAPLACE_NETWORK_INTERFACE_EXECUTION_H

#include "../defs.h"

namespace laplace::network {
  using fn_actor_reserve = std::function<void(span_cbyte)>;
  using fn_actor_create  = std::function<sl::index()>;
  using fn_actor_remove  = std::function<void(sl::index)>;
  using fn_do_perform    = std::function<void(span_cbyte)>;
  using fn_do_seed       = std::function<void(span_cbyte)>;
  using fn_do_loading    = std::function<void(span_cbyte)>;
  using fn_do_apply      = std::function<sl::time(span_cbyte)>;
  using fn_do_schedule   = std::function<void(sl::time)>;
  using fn_is_desync     = std::function<bool()>;
  using fn_is_ready      = std::function<bool()>;

  /*  Execution interface definition.
   *
   *  Relay should only implement actor_reserve, actor_create,
   *  actor_remove.
   */
  struct execution_interface {
    /*  Reserve identifiers for actors.
     *
     *  Required for relay to be able to create new actors without
     *  actual event execution.
     */
    fn_actor_reserve actor_reserve;

    /*  Returns a new actor identifier.
     */
    fn_actor_create actor_create;

    /*  Removes an actor.
     */
    fn_actor_remove actor_remove;

    /*  Perform an instant event.
     */
    fn_do_perform do_perform;
    
    /*  Seed the random number generator.
     */
    fn_do_seed do_seed;

    /*  Perform the loading. Called when execution is started.
     */
    fn_do_loading do_loading;

    /*  Apply an event. Returns the event time.
     */
    fn_do_apply do_apply;

    /*  Schedule the execution.
     */
    fn_do_schedule do_schedule;

    /*  Returns true if there was an error during execution.
     */
    fn_is_desync is_desync;
    
    /*  Returns true if there are no unfinished tasks. Required for
     *  asynchrony.
     */
    fn_is_ready is_ready;
  };

  [[nodiscard]] auto check_execution_interface(
      execution_interface const &in) noexcept -> bool;

  [[nodiscard]] auto blank_execution_interface() noexcept
      -> execution_interface;
}

#endif
