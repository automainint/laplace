/*  laplace/format/f_text.cpp
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

#include "text.h"

#include "../core/parser.h"
#include "../core/utils.h"
#include <iomanip>
#include <numeric>
#include <sstream>

namespace laplace::format::text {
  using std::make_shared, std::string, std::u8string, core::parser,
      core::unival, std::function, std::ostringstream, std::hex,
      std::numeric_limits, std::setprecision, std::string_view,
      std::u8string_view;

  static auto is_id_string(string_view s) noexcept -> bool {
    if (s.empty()) {
      return false;
    }

    if (s[0] >= '0' && s[0] <= '9') {
      return false;
    }

    for (auto c : s)
      if (c != '_' && (c < 'a' || c > 'z') && (c < 'A' || c > 'Z') &&
          (c < '0' || c > '9')) {
        return false;
      }

    return true;
  }

  static auto wrap_string(string_view s) noexcept -> string {
    auto result = string { "\"" };

    for (sl::index i = 0; i < s.size(); i++) {
      if (s[i] == '\\' || s[i] == '"') {
        result.append("\\");
      }

      result.append(1, s[i]);
    }

    result.append("\"");

    return result;
  }

  static auto unwrap_string(u8string_view s) noexcept -> u8string {
    auto result = u8string {};

    for (sl::index i = 0; i < s.size(); i++)
      if (s[i] != '\\' || ++i < s.size()) {
        result.append(1, s[i]);
      }

    return result;
  }

  static bool parse(parser &in,
                    unival &f,
                    bool    is_vec_elem = false) noexcept {
    auto field = unival {};

    auto x_u64 = uint64_t {};
    auto x_64  = int64_t {};
    auto x_f   = double {};
    auto x_s   = u8string {};

    auto parse_float = [&]() {
      in.push_offset();
      bool is_float = in.parse(" %~d.%~d") || in.parse(" %~de%~d") ||
                      in.parse(" %~de+%~D");
      in.pop_offset(true);

      return is_float && in.parse(" %f ", &x_f);
    };

    if (in.parse(" true ")) {
      field = true;
    } else if (in.parse(" false ")) {
      field = false;
    } else if (parse_float()) {
      field = x_f;
    } else if ((in.parse(" 0b") || in.parse(" 0B")) &&
               in.parse("%B ", &x_u64)) {
      field = x_u64;
    } else if ((in.parse(" -0b") || in.parse(" -0B")) &&
               in.parse("%B ", &x_u64)) {
      field = -static_cast<int64_t>(x_u64);
    } else if ((in.parse(" 0x") || in.parse(" 0X")) &&
               in.parse("%X ", &x_u64)) {
      field = x_u64;
    } else if ((in.parse(" -0x") || in.parse(" -0X")) &&
               in.parse("%X ", &x_u64)) {
      field = -static_cast<int64_t>(x_u64);
    } else if (in.parse(" 0%O ", &x_u64)) {
      field = x_u64;
    } else if (in.parse(" -0%O ", &x_u64)) {
      field = -static_cast<int64_t>(x_u64);
    } else if (in.parse(" %D ", &x_u64)) {
      field = x_u64;
    } else if (in.parse(" %d ", &x_64)) {
      field = x_64;
    } else if (in.parse(" \"%s\" ", &x_s)) {
      field = unwrap_string(x_s);
    } else if (in.parse(" %a ", &x_s)) {
      auto id = unival { x_s };

      if (in.parse(" ( ")) {
        field[text::s_function] = id;

        if (!in.parse(" ) ")) {
          auto args = unival {};

          if (!parse(in, args)) {
            return false;
          }

          if (args.is_vector()) {
            field[text::s_arguments] = args;
          } else {
            field[text::s_arguments][0] = args;
          }

          if (!in.parse(" ) ")) {
            return false;
          }
        }
      } else {
        field = id;
      }
    } else if (in.parse(" ( ")) {
      auto v = unival {};

      if (!parse(in, v)) {
        return false;
      }

      if (v.is_vector()) {
        field = v;
      } else {
        field[0] = v;
      }

      if (!in.parse(" ) ")) {
        return false;
      }

    } else if (in.parse(" { ")) {
      while (!in.parse(" } ")) {
        if (in.parse(" ; ")) {
          continue;
        }

        auto key   = unival {};
        auto value = unival {};

        if (!parse(in, key)) {
          return false;
        }

        if (key.is_composite() && key.has(s_function)) {
          auto &cmd           = field[text::s_commands];
          cmd[cmd.get_size()] = key;
          continue;
        }

        if ((!in.parse(" = ") && !in.parse(" : ")) ||
            !parse(in, value)) {
          return false;
        }

        field[key] = value;
      }
    } else if (in.parse(" :: { ")) {
      auto v = vbyte {};

      while (!in.parse(" } ")) {
        if (uint64_t x; in.parse(" %X ", &x) && x <= 0xff) {
          v.emplace_back(static_cast<uint8_t>(x));
        } else {
          return false;
        }
      }

      field = v;

    } else {
      return false;
    }

    if (is_vec_elem) {
      f[f.get_size()] = field;

    } else if (in.parse(" , ")) {
      f[0] = field;

      do { parse(in, f, true); } while (in.parse(" , "));

    } else {
      f = field;
    }

    return true;
  }

  auto decode(fn_read read) noexcept -> pack_type {
    if (!read)
      return {};

    auto in = parser([read]() -> char32_t {
      auto const c = read(1);
      if (c.size() == 1)
        return c[0];
      return 0;
    });

    if (auto f = unival {}; parse(in, f))
      return f;

    return {};
  }

  static bool printdown(function<bool(string_view)> print,
                        unival const &              f,
                        sl::whole                   indent = 0,
                        bool is_vec_elem = false) noexcept {
    if (f.is_boolean()) {
      return print(f ? "true" : "false");
    }

    if (f.is_integer()) {
      auto out = ostringstream {};
      out << f.get_integer();
      return print(out.str());
    }

    if (f.is_uint()) {
      auto out = ostringstream {};
      out << "0x" << hex << f.get_uint();
      return print(out.str());
    }

    if (f.is_real()) {
      auto out    = ostringstream {};
      auto digits = numeric_limits<double>::max_digits10;
      out << setprecision(digits) << f.get_real();
      return print(out.str());
    }

    if (f.is_string()) {
      auto v = string { as_ascii_string(f.get_string()) };

      if (is_id_string(v)) {
        return print(v);
      }

      v = wrap_string(v);

      return print(v);
    }

    if (f.is_bytes()) {
      if (!print(":: { ")) {
        return false;
      }

      auto size = f.get_size();

      for (sl::index i = 0; i < size; i++) {
        auto out = ostringstream {};
        out.width(2);
        out << hex << int { f.get_bytes()[i] } << " ";
        print(out.str());
      }

      return print("}");
    }

    if (f.is_vector()) {
      auto size = f.get_size();

      if (size <= 0) {
        return print("( )");
      }

      if (size == 1) {
        is_vec_elem = true;
      }

      if (is_vec_elem && !print("( ")) {
        return false;
      }

      for (sl::index i = 0; i < size; i++) {
        if (i > 0 && !print(", ")) {
          return false;
        }

        if (!printdown(print, f[i], indent, true)) {
          return false;
        }
      }

      if (is_vec_elem && !print(" )")) {
        return false;
      }

      return true;
    }

    if (f.is_composite()) {
      if (f.get_size() == 2 && f.has(s_function)) {

        if (!printdown(print, f[s_function], indent + 1) ||
            !print("(")) {
          return false;
        }

        if (f.has(s_arguments) &&
            !printdown(print, f[s_arguments], indent + 1)) {
          return false;
        }

        if (!print(")")) {
          return false;
        }

        return true;
      }

      if (!print("{\n"))
        return false;

      auto const size = f.get_size();

      for (sl::index i = 0; i < size; i++) {
        for (sl::index n = 0; n <= indent; n++)
          if (!print("  ")) {
            return false;
          }

        auto const &k = f.get_key(i);

        if (k.get_string() == s_commands) {
          auto const &cmds = f[k];

          for (sl::index j = 0; j < cmds.get_size(); j++)
            if (!printdown(print, cmds[j], indent + 1)) {
              return false;
            }

        } else {
          if (!printdown(print, k, indent + 1)) {
            return false;
          }

          if (f[k].is_composite()) {
            if (!print(": ")) {
              return false;
            }
          } else if (!print(" = ")) {
            return false;
          }

          if (!printdown(print, f[k], indent + 1)) {
            return false;
          }
        }
      }

      for (sl::index n = 0; n < indent; n++)
        if (!print("  ")) {
          return false;
        }

      return print("}");
    }

    return print("{ }");
  }

  auto encode(fn_write write, const_pack_type data) noexcept -> bool {
    auto print = [write](string_view s) -> bool {
      auto v = vbyte(s.size());
      memcpy(v.data(), s.data(), v.size());
      return write(v) == v.size();
    };

    return printdown(print, data);
  }
}
