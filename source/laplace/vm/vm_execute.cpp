/*  laplace/vm/vm_execute.cpp
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

#include "../core/utils.h"
#include "bitcode.h"
#include "instance.h"

namespace laplace::vm::execute {
  auto read_inst(cref_vbyte code, ref_state s) -> uint8_t {
    if (s.instruction_pointer >= code.size())
      return bitcode::i_abort;

    return code[s.instruction_pointer++];
  }

  auto read_arg(cref_vbyte code, ref_state s, ref_heap heap)
      -> uint64_t {
    if (s.instruction_pointer < code.size()) {
      const auto x = code[s.instruction_pointer++];

      const int size = x & 0xf;
      const int mode = (x & 0xf0) >> 1;

      if (mode == bitcode::a_const) {
        return read_const(code, s, size);
      } else if (mode == bitcode::a_stack) {
        return read_stack(
            s.stack, size, as_index(read_arg(code, s, heap)));
      } else if (mode == bitcode::a_heap) {
        return read_heap(heap, size, as_index(read_arg(code, s, heap)));
      } else {
        error(__FUNCTION__, "Invalid mode.");
      }
    } else {
      error(__FUNCTION__, "End of code.");
    }

    return 0;
  }

  auto read_const(cref_vbyte code, ref_state s, int size) -> uint64_t {
    if (size == bitcode::s_byte) {
      if (s.instruction_pointer < code.size()) {
        return code[s.instruction_pointer++];
      } else {
        error(__FUNCTION__, "End of code.");
      }
    } else if (size == bitcode::s_word) {
      if (s.instruction_pointer + sizeof(uint16_t) <= code.size()) {
        const auto x = rd<uint16_t>(code, s.instruction_pointer);
        s.instruction_pointer += sizeof x;
        return x;
      } else {
        error(__FUNCTION__, "End of code.");
      }
    } else if (size == bitcode::s_dword) {
      if (s.instruction_pointer + sizeof(uint32_t) <= code.size()) {
        const auto x = rd<uint32_t>(code, s.instruction_pointer);
        s.instruction_pointer += sizeof x;
        return x;
      } else {
        error(__FUNCTION__, "End of code.");
      }
    } else if (size == bitcode::s_qword) {
      if (s.instruction_pointer + sizeof(uint64_t) <= code.size()) {
        const auto x = rd<uint64_t>(code, s.instruction_pointer);
        s.instruction_pointer += sizeof x;
        return x;
      } else {
        error(__FUNCTION__, "End of code.");
      }
    } else {
      error(__FUNCTION__, "Invalid size.");
    }

    return 0;
  }

  auto read_stack(cref_vbyte stack, int size, size_t offset)
      -> uint64_t {
    if (size == bitcode::s_byte) {
      return rd<uint8_t>(stack, offset);
    } else if (size == bitcode::s_word) {
      return rd<uint16_t>(stack, offset);
    } else if (size == bitcode::s_dword) {
      return rd<uint32_t>(stack, offset);
    } else if (size == bitcode::s_qword) {
      return rd<uint64_t>(stack, offset);
    } else {
      error(__FUNCTION__, "Invalid size.");
    }

    return 0;
  }

  auto read_heap(ref_heap heap, int size, size_t address) -> uint64_t {
    if (size == bitcode::s_byte) {
      uint8_t x = 0;
      heap.read(address, { &x, 1 });
      return x;
    } else if (size == bitcode::s_word) {
      uint8_t x[2] = { 0 };
      heap.read(address, x);
      return rd<uint16_t>(x, 0);
    } else if (size == bitcode::s_dword) {
      uint8_t x[4] = { 0 };
      heap.read(address, x);
      return rd<uint32_t>(x, 0);
    } else if (size == bitcode::s_qword) {
      uint8_t x[8] = { 0 };
      heap.read(address, x);
      return rd<uint64_t>(x, 0);
    } else {
      error(__FUNCTION__, "Invalid size.");
    }

    return 0;
  }

  void write_stack(ref_vbyte stack, int size, uint64_t value,
                   size_t address) {
    error(__FUNCTION__, "Not implemented.");
  }

  void write_heap(ref_heap heap, int size, uint64_t value,
                  size_t address) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_return(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_call(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_jump(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_jump_if(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_push(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_pop(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_move(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_swap(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_alloc(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_less(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_larger(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_equals(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_not_less(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_not_larger(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_not_equals(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_and(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_or(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_unsigned_less(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_unsigned_larger(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_unsigned_not_less(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_unsigned_not_larger(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_bit_not(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_bit_add(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_bit_or(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_bit_xor(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_bit_shl(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_bit_shr(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_neg(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_add(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_sub(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_mul(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_div(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_umul(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_udiv(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_abs(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_clamp(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_fix_scale(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_fix_mul(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_fix_div(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_fix_floor(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_fix_ceil(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_fix_sqrt(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_fix_pow(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_fix_exp(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_fix_log(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_fix_sin(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  void do_fix_cos(ref_state s, ref_heap heap) {
    error(__FUNCTION__, "Not implemented.");
  }

  auto tick(ref_state s, cref_vbyte code, ref_heap heap, fn_fork fork,
            fn_call_external call_ext, fn_export exp,
            size_t instr_limit) -> size_t {
    error(__FUNCTION__, "Not implemented.");
    s.is_active = false;
    return 0;
  }
}
