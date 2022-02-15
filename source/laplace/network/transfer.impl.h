/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_NETWORK_TRANSFER_IMPL_H
#define LAPLACE_NETWORK_TRANSFER_IMPL_H

namespace laplace::network {
  inline auto transfer::wrap(span_cbyte arg0, auto... args) noexcept
      -> sl::vector<span_cbyte> {
    auto v = sl::vector<span_cbyte> {};
    v.reserve(1 + sizeof...(args));
    _append(v, arg0, args...);
    return v;
  }

  inline auto transfer::wrap(sl::vector<vbyte> const &reqs) noexcept
      -> sl::vector<span_cbyte> {
    auto v = sl::vector<span_cbyte> {};
    v.reserve(reqs.size());
    for (auto req : reqs) v.emplace_back(req);
    return v;
  }

  inline void transfer::_append(sl::vector<span_cbyte> &v) noexcept {
  }

  inline void transfer::_append(sl::vector<span_cbyte> &v, auto arg0,
                                auto... args) noexcept {
    v.emplace_back(arg0);
    _append(v, args...);
  }
}

#endif
