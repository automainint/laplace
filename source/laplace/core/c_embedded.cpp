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
  std::shared_mutex g_lock;

  static bool is_ready = false;

  extern std::vector<std::wstring>         aliases;
  extern std::vector<std::vector<uint8_t>> bytes;

  void init() {
    if (!is_ready) {
      auto size = aliases.size();

      vuint indices(size);

      for (size_t i = 0; i < size; i++)
        indices[i] = i;

      sort(indices.begin(), indices.end(),
          [](size_t i, size_t j) -> bool {
            return aliases[i] < aliases[j];
          });

      std::vector<std::wstring>         sorted_aliases(size);
      std::vector<std::vector<uint8_t>> sorted_bytes(size);

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

  auto scan(std::wstring_view file_name) -> bool {
    return !file_name.empty() && file_name[0] == L':';
  }

  auto exists(std::wstring_view file_name) -> bool {
    auto _sl = std::shared_lock(g_lock);

    init();

    auto i = std::lower_bound(
        aliases.begin(), aliases.end(), file_name);
    return i != aliases.end() && *i == file_name;
  }

  auto open(std::wstring_view file_name) -> cref_vbyte {
    auto _ul = std::unique_lock(g_lock);

    init();

    auto i =
        lower_bound(aliases.begin(), aliases.end(), file_name);

    if (i != aliases.end() && *i == file_name) {
      return bytes[i - aliases.begin()];
    }

    return {};
  }
}
