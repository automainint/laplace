#include "state.h"

#include "impact.h"

namespace laplace {
  using std::random_device, std::make_shared, std::shared_ptr;

  state::state() noexcept : m_io(make_shared<io_impl>()) {
    std::ignore = m_io->apply(
        impact { integer_seed { random_device {}() } });
  }

  state::state(shared_ptr<io_interface> io) noexcept :
      m_io(std::move(io)) { }

  state::state(state const &other) noexcept {
    m_io = other.m_io->clone();
  }

  auto state::operator=(state const &other) noexcept -> state & {
    if (this != &other)
      m_io = other.m_io->clone();
    return *this;
  }

  auto state::io() const noexcept -> shared_ptr<io_interface> {
    return m_io;
  }

  auto state::get_integer(ptrdiff_t id, ptrdiff_t index,
                          int_type def) const noexcept -> int_type {
    return m_io->get_integer(id, index, def);
  }

  auto state::get_byte(ptrdiff_t id, ptrdiff_t index,
                       byte_type def) const noexcept -> byte_type {
    return m_io->get_byte(id, index, def);
  }

  auto state::apply(impact const &i) noexcept -> bool {
    return m_io->apply(i);
  }

  auto state::adjust() noexcept -> bool {
    return m_io->adjust();
  }

  void state::adjust_done() noexcept {
    m_io->adjust_done();
  }
}
