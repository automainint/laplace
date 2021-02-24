/*  laplace/vm/bitcode.h
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

#ifndef laplace_vm_bitcode_h
#define laplace_vm_bitcode_h

#include <cstdint>

namespace laplace::vm::bitcode {
  /*  Instruction name.
   *  8-bit value.
   */
  enum instruction : uint8_t {
    /*  Control flow.
     */

    /*  Stop execution.
     */
    i_abort,

    /*  Return control.
     */
    i_yeild,

    /*  A function entry point.
     */
    i_entry,

    /*  Return control.
     *
     *  Args:
     *      size        - return value size
     *
     *  Stack:
     *  pop size        - store return value
     *  pop 8           - return address
     *  [push size]     - put return value, if internal return
     *
     *      If no return address, exit
     *      to external flow.
     */
    i_return,

    /*  Call function.
     *
     *  Args:
     *      func        - function address
     *
     *  Stack:
     *  push 8          - return address
     */
    i_call,

    /*  Call external function.
     *
     *  Args:
     *      func        - function id.
     *      size        - function arguments size.
     *
     *  Stack:
     *  push N          - function return value.
     */
    i_call_external,

    /*  Create thread.
     *
     *  Args:
     *      func        - function address.
     */
    i_fork,

    /*  Jump
     *
     *  Args:
     *      addr        - instruction address.
     */
    i_jump,

    /*  Conditional jump
     *
     *  Args:
     *      addr        - instruction address.
     *      cond        - condition flag.
     */
    i_jump_if,

    /*  Import function id
     *
     *  Args:
     *      id          - unique function id.
     *      id_ex       - function id: stack, heap or ptr.
     */
    i_import,

    /*  Export function id
     *
     *  Args:
     *      id          - unique function id.
     *      addr        - function address.
     */
    i_export,

    /*  Stack.
     */

    /*  Push
     *
     *  Args:
     *      value       - const, stack, heap or ptr.
     *      count
     */
    i_push,

    /*  Pop
     *
     *  Args:
     *      value       - stack, heap or ptr.
     *      count
     */
    i_pop,

    /*  Memory.
     */

    /*  Move value.
     *
     *  Args:
     *      from        - const, stack, heap or ptr.
     *      to          - stack, heap or ptr.
     *      count       - value count.
     */
    i_move,

    /*  Swap values.
     *
     *  Args:
     *      from        - stack, heap or ptr.
     *      to          - stack, heap or ptr.
     *      count       - value count.
     */
    i_swap,

    /*  Allocate memory. If addr is -1, allocates a new
     *  block. If addr isn't -1, reallocates the block. If
     *  size is 0, deallocate the block.
     *
     *  Args:
     *      addr        - memory address: stack, heap or ptr.
     *      size        - byte count to allocate.
     */
    i_alloc,

    /*  Boolean arithmetics.
     */

    i_less,
    i_larger,
    i_equals,
    i_not_less,
    i_not_larger,
    i_not_equals,
    i_and,
    i_or,
    i_unsigned_less,
    i_unsigned_larger,
    i_unsigned_not_less,
    i_unsigned_not_larger,

    /*  Bit arithmetics.
     */

    i_bit_not,
    i_bit_and,
    i_bit_or,
    i_bit_xor,
    i_bit_shl,
    i_bit_shr,

    /*  Integer arithmetics.
     */

    i_negative,
    i_add,
    i_subtract,
    i_multiply,
    i_divide,
    i_unsigned_multiply,
    i_unsigned_subtract,
    i_absolute,
    i_clamp,

    /*  Set fixed-point scale.
     *
     *  Args:
     *      scale       - fixed-point scale.
     */
    i_fix_scale,

    /*  Fixed-point math.
     */

    i_fix_multiply,
    i_fix_divide,
    i_fix_floor,
    i_fix_ceiling,
    i_fix_sqrt,
    i_fix_power,
    i_fix_exp,
    i_fix_log,
    i_fix_sin,
    i_fix_cos,

    _instruction_count
  };

  /*  Instruction argument size.
   *  4-bit value.
   */
  enum argument_size : uint8_t {
    s_byte,
    s_word,
    s_dword,
    s_qword,

    s_invalid = 0xf
  };

  /*  Instruction argument mode.
   *  4-bit value.
   */
  enum argument_mode : uint8_t {
    /*  Seek argument from bitcode after current command.
     */
    a_const,

    /*  Seek argument in stack. Value after current command
     *  is an argument determining the stack offset.
     */
    a_stack,

    /*  Seek argument from heap array start. Value after
     *  current command is an argument determining the
     *  array offset.
     */
    a_heap,

    a_invalid = 0xf
  };
}

#endif
