#ifndef LAPLACE_STATE_H
#define LAPLACE_STATE_H

#include "io_impl.h"

namespace laplace {
  class impact;

  class state {
  public:
    state() noexcept;
    explicit state(std::shared_ptr<io_interface> io) noexcept;

    state(state const &other) noexcept;
    state(state &&other) noexcept = default;
    ~state() noexcept             = default;
    auto operator=(state const &other) noexcept -> state &;
    auto operator=(state &&other) noexcept -> state & = default;

    [[nodiscard]] auto io() const noexcept
        -> std::shared_ptr<io_interface>;

    [[nodiscard]] auto get_integer(ptrdiff_t id, ptrdiff_t index,
                                   int_type def) const noexcept
        -> int_type;

    [[nodiscard]] auto get_byte(ptrdiff_t id, ptrdiff_t index,
                                byte_type def) const noexcept
        -> byte_type;

    [[nodiscard]] auto apply(impact const &i) noexcept -> bool;

    [[nodiscard]] auto adjust() noexcept -> bool;

    void adjust_done() noexcept;

  private:
    std::shared_ptr<io_interface> m_io;
  };
}

#endif
