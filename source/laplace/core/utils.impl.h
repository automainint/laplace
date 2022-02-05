/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_core_utils_impl_h
#define laplace_core_utils_impl_h

namespace laplace::impl {
  constexpr auto as_index_unsigned(unsigned long long const value,
                                   sl::index const          invalid,
                                   bool const silent) noexcept
      -> sl::index {

    if constexpr (!_unsafe) {
      if (value == -1) {
        return invalid;
      }

      if (value > std::numeric_limits<sl::index>::max()) {
        if (!silent)
          error_("Index value overflow.", __FUNCTION__);
        return invalid;
      }
    }

    return static_cast<sl::index>(value);
  }

  constexpr auto as_index_signed(signed long long const value,
                                 sl::index const        invalid,
                                 bool const silent) noexcept
      -> sl::index {

    if constexpr (!_unsafe) {
      if (value == -1) {
        return invalid;
      }

      if constexpr (sizeof value > sizeof(sl::index)) {
        if (value < 0 ||
            value > std::numeric_limits<sl::index>::max()) {
          if (!silent)
            error_("Index value overflow.", __FUNCTION__);
          return invalid;
        }
      } else {
        if (value < 0) {
          if (!silent)
            error_("Index value overflow.", __FUNCTION__);
          return invalid;
        }
      }
    }

    return static_cast<sl::index>(value);
  }
}

namespace laplace {
  constexpr auto as_index(unsigned char const value,
                          sl::index const     invalid,
                          bool const silent) noexcept -> sl::index {
    return impl::as_index_unsigned(value, invalid, silent);
  }

  constexpr auto as_index(unsigned short const value,
                          sl::index const      invalid,
                          bool const silent) noexcept -> sl::index {
    return impl::as_index_unsigned(value, invalid, silent);
  }

  constexpr auto as_index(unsigned int const value,
                          sl::index const    invalid,
                          bool const silent) noexcept -> sl::index {
    return impl::as_index_unsigned(value, invalid, silent);
  }

  constexpr auto as_index(unsigned long const value,
                          sl::index const     invalid,
                          bool const silent) noexcept -> sl::index {
    return impl::as_index_unsigned(value, invalid, silent);
  }

  constexpr auto as_index(unsigned long long const value,
                          sl::index const          invalid,
                          bool const silent) noexcept -> sl::index {
    return impl::as_index_unsigned(value, invalid, silent);
  }

  constexpr auto as_index(signed char const value,
                          sl::index const   invalid,
                          bool const silent) noexcept -> sl::index {
    return impl::as_index_signed(value, invalid, silent);
  }

  constexpr auto as_index(signed short const value,
                          sl::index const    invalid,
                          bool const silent) noexcept -> sl::index {
    return impl::as_index_signed(value, invalid, silent);
  }

  constexpr auto as_index(signed int const value,
                          sl::index const  invalid,
                          bool const silent) noexcept -> sl::index {
    return impl::as_index_signed(value, invalid, silent);
  }

  constexpr auto as_index(signed long const value,
                          sl::index const   invalid,
                          bool const silent) noexcept -> sl::index {
    return impl::as_index_signed(value, invalid, silent);
  }

  constexpr auto as_index(signed long long const value,
                          sl::index const        invalid,
                          bool const silent) noexcept -> sl::index {
    return impl::as_index_signed(value, invalid, silent);
  }
}

#endif
