/*  Copyright (c) 2022 Mitya Selivanov
 */

#ifndef LAPLACE_IO_INTERFACE_H
#define LAPLACE_IO_INTERFACE_H

#include "options.h"
#include <memory>

namespace laplace {
  class impact;

  struct io_interface {
    virtual ~io_interface() noexcept = default;

    [[nodiscard]] virtual auto clone() const noexcept
        -> std::shared_ptr<io_interface> = 0;

    /*  Get an integer value.
     *  Can be called asynchronously.
     */
    [[nodiscard]] virtual auto get_integer(
        ptrdiff_t id, ptrdiff_t index, int_type def) const noexcept
        -> int_type = 0;

    /*  Get a byte value.
     *  Can be called asynchronously.
     */
    [[nodiscard]] virtual auto get_byte(ptrdiff_t id, ptrdiff_t index,
                                        byte_type def) const noexcept
        -> byte_type = 0;

    /*  Apply an impact.
     *  Some impacts can be applied asynchronously.
     *  Can have a side effect.
     */
    [[nodiscard]] virtual auto apply(impact const &i) noexcept
        -> bool = 0;

    /*  Adjust all values according to applied side effects.
     *  Can be parallelized.
     *  Returns true while adjusting is not done.
     *
     *  Usage, loop can be parallelized:
     *
     *    while (io->adjust()) { }
     *    io->adjust_done();
     */
    [[nodiscard]] virtual auto adjust() noexcept -> bool = 0;

    /*  Adjust is done.
     */
    virtual void adjust_done() noexcept = 0;
  };
}

#endif
