/*  laplace/core/c_embedded.cpp
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

#include "embedded.h"
#include <algorithm>
#include <shared_mutex>

namespace laplace::embedded {
  using std::lower_bound, std::sort, std::shared_lock,
      std::unique_lock, std::shared_mutex, std::vector, std::wstring,
      std::wstring_view;

  shared_mutex g_lock;

  static bool is_ready = false;

  extern vector<wstring>         aliases;
  extern vector<vector<uint8_t>> bytes;

  void init() {
    if (!is_ready) {
      auto size = aliases.size();

      auto indices = vuint(size);

      for (size_t i = 0; i < size; i++) indices[i] = i;

      sort(indices.begin(), indices.end(),
           [](size_t i, size_t j) -> bool {
             return aliases[i] < aliases[j];
           });

      auto sorted_aliases = vector<wstring>(size);
      auto sorted_bytes   = vector<vector<uint8_t>>(size);

      if (bytes.size() != size)
        bytes.resize(size);

      for (size_t i = 0; i < size; i++) {
        auto n = indices[i];

        sorted_aliases[i] = aliases[n];
        sorted_bytes[i]   = bytes[n];
      }

      aliases = sorted_aliases;
      bytes   = sorted_bytes;

      is_ready = true;
    }
  }

  auto scan(wstring_view file_name) noexcept -> bool {
    return !file_name.empty() && file_name[0] == L':';
  }

  auto exists(wstring_view file_name) -> bool {
    auto _sl = shared_lock(g_lock);

    init();

    auto i = lower_bound(aliases.begin(), aliases.end(), file_name);
    return i != aliases.end() && *i == file_name;
  }

  auto open(wstring_view file_name) -> span_cbyte {
    auto _ul = unique_lock(g_lock);

    init();

    auto i = lower_bound(aliases.begin(), aliases.end(), file_name);

    if (i != aliases.end() && *i == file_name) {
      return bytes[static_cast<size_t>(i - aliases.begin())];
    }

    return {};
  }
}
