#include "x_io_impl.h"

#include "x_impact.h"

namespace laplace {
  using std::visit, std::is_same_v, std::decay_t,
      std::uniform_int_distribution, std::shared_ptr,
      std::make_shared;

  auto io_impl::clone() const noexcept -> shared_ptr<io_interface> {
    return make_shared<io_impl>(*this);
  }

  auto io_impl::get_integer(ptrdiff_t id, ptrdiff_t index,
                            int_type def) const noexcept -> int_type {
    return integers.get(id, index, def);
  }

  auto io_impl::get_byte(ptrdiff_t id, ptrdiff_t index,
                         byte_type def) const noexcept -> byte_type {
    return bytes.get(id, index, def);
  }

  auto io_impl::apply(impact const &i) noexcept -> bool {
    return visit(
        [&](auto const &i) {
          using type = decay_t<decltype(i)>;

          if constexpr (is_same_v<type, noop>)
            return true;

          else if constexpr (is_same_v<type, integer_reserve>)
            return integers.reserve(i.count);

          else if constexpr (is_same_v<type, integer_allocate_into>)
            return integers.allocate_into(i.id, i.size);

          else if constexpr (is_same_v<type, integer_allocate>) {
            auto const id = integers.allocate(i.size);
            return id != id_undefined
                       ? integers.set(i.return_id, i.return_index, id)
                       : false;
          }

          else if constexpr (is_same_v<type, integer_deallocate>)
            return integers.deallocate(i.id);

          else if constexpr (is_same_v<type, integer_set>)
            return integers.set(i.id, i.index, i.value);

          else if constexpr (is_same_v<type, integer_add>)
            return integers.add(i.id, i.index, i.delta);

          else if constexpr (is_same_v<type, byte_reserve>)
            return bytes.reserve(i.count);

          else if constexpr (is_same_v<type, byte_allocate_into>)
            return bytes.allocate_into(i.id, i.size);

          else if constexpr (is_same_v<type, byte_allocate>) {
            auto const id = bytes.allocate(i.size);
            return id != id_undefined
                       ? integers.set(i.return_id, i.return_index, id)
                       : false;
          }

          else if constexpr (is_same_v<type, byte_deallocate>)
            return bytes.deallocate(i.id);

          else if constexpr (is_same_v<type, byte_set>)
            return bytes.set(i.id, i.index, i.value);

          else if constexpr (is_same_v<type, byte_add>)
            return bytes.add(i.id, i.index, i.delta);

          else if constexpr (is_same_v<type, integer_seed>) {
            random.seed(i.seed);
            return true;
          }

          else if constexpr (is_same_v<type, integer_random>) {
            auto distribute = uniform_int_distribution<int_type> {
              i.min, i.max
            };
            for (ptrdiff_t k = 0; k < i.return_size; k++)
              if (!integers.set(i.return_id, i.return_index + k,
                                distribute(random)))
                return false;
            return true;
          }

          return false;
        },
        i.value);
  }

  auto io_impl::adjust() noexcept -> bool {
    return integers.adjust() || bytes.adjust();
  }

  void io_impl::adjust_done() noexcept {
    integers.adjust_done();
    bytes.adjust_done();
  }
}
