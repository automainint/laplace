/*  laplace/vm/heap.h
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

#ifndef __laplace__vm_heap__
#define __laplace__vm_heap__

#include "../core/defs.h"
#include <shared_mutex>

namespace laplace::vm {
  /*  Thread-safe heap implementation for
   *  virtual machine instance.
   */
  class heap {
  public:
    static constexpr size_t invalid_address = -1;

    /*  Allocate memory. Allocates a new block if addr is null,
     *  reallocates the block if addr isn't null, deallocates
     *  the block if size is zero. Returns block addr (-1 for
     *  invalid address).
     */
    auto allocate(size_t addr, size_t size) -> size_t;

    void read(size_t addr, std::span<uint8_t> data);
    void write(size_t addr, std::span<const uint8_t> data);

  private:
    auto do_alloc(size_t size) -> size_t;
    auto do_realloc(size_t offset, size_t size) -> size_t;
    void do_dealloc(size_t offset);

    auto find_place(size_t size) -> size_t;
    auto is_empty(size_t offset, size_t size) -> bool;

    class block {
    public:
      std::shared_mutex lock;
      size_t            offset = 0;
      size_t            size   = 0;

      block() = default;
      block(size_t _offset, size_t _size);
      block(const block &b);
      block(block &&b) noexcept;
      auto operator=(const block &b);
      auto operator=(block &&b) noexcept;
    };

    static auto op_left(const block &b, size_t offset) -> bool;
    static auto op_right(const block &b, size_t offset) -> bool;

    std::shared_mutex  m_lock;
    std::vector<block> m_blocks;
    vbyte              m_data;
  };

  using ref_heap  = heap &;
  using cref_heap = const heap &;
}

#endif
