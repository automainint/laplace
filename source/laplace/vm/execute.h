#pragma once

#include "heap.h"
#include <functional>

namespace laplace::vm::execute
{
    /*  Default fixed-point arithmetic scale factor.
     */
    constexpr uint64_t default_fix_scale = 1000;

    /*  Single tick instructions limit.
     */
    constexpr uint64_t default_instructions_limit = -1;

    using fn_fork           = std::function<void(size_t address, cref_vbyte args)>;
    using fn_call_external  = std::function<vbyte(size_t id, cref_vbyte args)>;
    using fn_export         = std::function<void(size_t id, size_t address)>;

    struct state
    {
        bool        is_active           = false;
        size_t      instruction_pointer = 0;
        uint64_t   fix_scale           = default_fix_scale;
        vbyte       stack;
    };

    using ref_state = state &;

    struct arg_state
    {
        int         size;
        int         mode;
        uint64_t    value;
    };

    auto read_inst(ref_state s) -> uint8_t;
    auto read_arg(ref_state s, ref_heap heap) -> arg_state;

    auto read_value(ref_state s, int size) -> uint64_t;

    auto read_stack_8(cref_vbyte stack, int offset) -> uint8_t;
    auto read_stack_16(cref_vbyte stack, int offset) -> uint16_t;
    auto read_stack_32(cref_vbyte stack, int offset) -> uint32_t;
    auto read_stack_64(cref_vbyte stack, int offset) -> uint64_t;

    auto read_heap_8(cref_heap heap, size_t address) -> uint8_t;
    auto read_heap_16(cref_heap heap, size_t address) -> uint16_t;
    auto read_heap_32(cref_heap heap, size_t address) -> uint32_t;
    auto read_heap_64(cref_heap heap, size_t address) -> uint64_t;

    void write_stack(ref_vbyte stack, uint8_t value, size_t address);
    void write_stack(ref_vbyte stack, uint16_t value, size_t address);
    void write_stack(ref_vbyte stack, uint32_t value, size_t address);
    void write_stack(ref_vbyte stack, uint64_t value, size_t address);

    void write_heap(ref_heap heap, uint8_t value, size_t address);
    void write_heap(ref_heap heap, uint16_t value, size_t address);
    void write_heap(ref_heap heap, uint32_t value, size_t address);
    void write_heap(ref_heap heap, uint64_t value, size_t address);

    void do_return(ref_state s, ref_heap heap);
    void do_call(ref_state s, ref_heap heap);
    void do_jump(ref_state s, ref_heap heap);
    void do_jump_if(ref_state s, ref_heap heap);
    void do_push(ref_state s, ref_heap heap);
    void do_pop(ref_state s, ref_heap heap);
    void do_move(ref_state s, ref_heap heap);
    void do_swap(ref_state s, ref_heap heap);
    void do_alloc(ref_state s, ref_heap heap);

    void do_less(ref_state s, ref_heap heap);
    void do_larger(ref_state s, ref_heap heap);
    void do_equals(ref_state s, ref_heap heap);
    void do_not_less(ref_state s, ref_heap heap);
    void do_not_larger(ref_state s, ref_heap heap);
    void do_not_equals(ref_state s, ref_heap heap);
    void do_and(ref_state s, ref_heap heap);
    void do_or(ref_state s, ref_heap heap);
    void do_unsigned_less(ref_state s, ref_heap heap);
    void do_unsigned_larger(ref_state s, ref_heap heap);
    void do_unsigned_not_less(ref_state s, ref_heap heap);
    void do_unsigned_not_larger(ref_state s, ref_heap heap);

    void do_bit_not(ref_state s, ref_heap heap);
    void do_bit_add(ref_state s, ref_heap heap);
    void do_bit_or(ref_state s, ref_heap heap);
    void do_bit_xor(ref_state s, ref_heap heap);
    void do_bit_shl(ref_state s, ref_heap heap);
    void do_bit_shr(ref_state s, ref_heap heap);

    void do_neg(ref_state s, ref_heap heap);
    void do_add(ref_state s, ref_heap heap);
    void do_sub(ref_state s, ref_heap heap);
    void do_mul(ref_state s, ref_heap heap);
    void do_div(ref_state s, ref_heap heap);
    void do_umul(ref_state s, ref_heap heap);
    void do_udiv(ref_state s, ref_heap heap);
    void do_abs(ref_state s, ref_heap heap);
    void do_clamp(ref_state s, ref_heap heap);

    void do_fix_scale(ref_state s, ref_heap heap);
    void do_fix_mul(ref_state s, ref_heap heap);
    void do_fix_div(ref_state s, ref_heap heap);
    void do_fix_floor(ref_state s, ref_heap heap);
    void do_fix_ceil(ref_state s, ref_heap heap);
    void do_fix_sqrt(ref_state s, ref_heap heap);
    void do_fix_pow(ref_state s, ref_heap heap);
    void do_fix_exp(ref_state s, ref_heap heap);
    void do_fix_log(ref_state s, ref_heap heap);
    void do_fix_sin(ref_state s, ref_heap heap);
    void do_fix_cos(ref_state s, ref_heap heap);

    auto tick(
        ref_state           s,
        ref_heap            heap,
        fn_fork             fork,
        fn_call_external    call_ext,
        fn_export           exp,
        size_t              instr_limit
    ) -> size_t;
}
