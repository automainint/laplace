#include "heap.h"
#include <algorithm>

using namespace laplace;
using namespace vm;
using namespace std;

auto heap::allocate(size_t addr, size_t size) -> size_t {
  size_t result = 0;

  if (size > 0) {
    size_t block_size = 1;
    while (block_size < size)
      block_size <<= 1;

    if (addr == 0) {
      result = do_alloc(block_size);
    } else {
      result = do_realloc(addr, block_size);
    }
  } else if (addr != 0) {
    do_dealloc(addr);
  }

  return result;
}

void heap::read(size_t addr, size_t size, uint8_t *data) {
  size_t n0 = addr;
  size_t n1 = n0 + size;

  auto _sl = shared_lock(m_lock);

  if (n0 > n1 || n1 > m_data.size()) {
    error(__FUNCTION__, "Invalid address.");
    return;
  }

  auto i = lower_bound(
      m_blocks.begin(), m_blocks.end(), n0, op_right);

  if (i != m_blocks.end()) {
    auto _sl_block = shared_lock(i->lock);

    if (n1 > i->offset + i->size) {
      error(__FUNCTION__, "Access violation.");
      return;
    }

    copy(m_data.data() + n0, m_data.data() + n1, data);
  }
}

void heap::write(
    size_t addr, size_t size, const uint8_t *data) {
  size_t n0 = addr;
  size_t n1 = n0 + size;

  auto _sl = shared_lock(m_lock);

  if (n0 > n1 || n1 > m_data.size()) {
    error(__FUNCTION__, "Invalid address.");
    return;
  }

  auto i = lower_bound(
      m_blocks.begin(), m_blocks.end(), n0, op_right);

  if (i != m_blocks.end()) {
    auto _ul_block = unique_lock(i->lock);

    if (n1 > i->offset + i->size) {
      error(__FUNCTION__, "Access violation.");
      return;
    }

    copy(data, data + size, m_data.data() + n0);
  }
}

auto heap::do_alloc(size_t size) -> size_t {
  auto _ul = unique_lock(m_lock);

  auto offset = find_place(size);

  auto i = lower_bound(
      m_blocks.begin(), m_blocks.end(), offset, op_left);

  m_blocks.emplace(i, block { offset, size });

  return offset;
}

auto heap::do_realloc(size_t offset, size_t size) -> size_t {
  auto _ul = unique_lock(m_lock);

  auto i = lower_bound(
      m_blocks.begin(), m_blocks.end(), offset, op_left);

  if (i != m_blocks.end() && i->offset == offset) {
    if (size > i->size &&
        !is_empty(i->offset + i->size, size - i->size)) {
      auto i0 = m_data.begin() + i->offset;
      auto i1 = m_data.begin() + i->size;

      m_blocks.erase(i);

      offset = find_place(size);

      move(i0, i1, m_data.begin() + offset);

      auto j = lower_bound(
          m_blocks.begin(), m_blocks.end(), offset, op_left);

      m_blocks.emplace(j, block { offset, size });
    } else {
      i->size = size;
    }
  } else {
    error(__FUNCTION__, ": Invalid reallocation address.");
    offset = invalid_address;
  }

  return offset;
}

void heap::do_dealloc(size_t offset) {
  auto _ul = unique_lock(m_lock);

  auto i = lower_bound(
      m_blocks.begin(), m_blocks.end(), offset, op_left);

  if (i != m_blocks.end() && i->offset == offset) {
    m_blocks.erase(i);
  } else {
    error(__FUNCTION__, "Invalid deallocation address.");
  }
}

auto heap::find_place(size_t size) -> size_t {
  size_t offset = 0;

  for (; offset < m_data.size(); offset += size) {
    if (is_empty(offset, size))
      break;
  }

  if (offset + size > m_data.size()) {
    m_data.resize(offset + size);
  }

  return offset;
}

auto heap::is_empty(size_t offset, size_t size) -> bool {
  auto i = lower_bound(
      m_blocks.begin(), m_blocks.end(), offset, op_right);

  return i == m_blocks.end() || offset + size <= i->offset;
}

heap::block::block(size_t _offset, size_t _size) :
    offset(_offset), size(_size) { }

heap::block::block(const heap::block &b) :
    offset(b.offset), size(b.size) { }

heap::block::block(heap::block &&b) noexcept :
    offset(b.offset), size(b.size) { }

auto heap::block::operator=(const heap::block &b) {
  offset = b.offset;
  size   = b.size;
  return *this;
}

auto heap::block::operator=(heap::block &&b) noexcept {
  offset = b.offset;
  size   = b.size;
  return *this;
}

auto heap::op_left(const heap::block &b, size_t offset)
    -> bool {
  return b.offset < offset;
}

auto heap::op_right(const heap::block &b, size_t offset)
    -> bool {
  return b.offset + b.size <= offset;
}
