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

#include "../core/parser.h"
#include "../core/utils.h"
#include "text.h"
#include <iomanip>
#include <sstream>

namespace laplace::format::text {
  using std::make_shared, std::string, std::u8string, core::parser,
      core::family, std::function, std::ostringstream, std::hex;

  static bool parse(parser &in, family &f) {
    bool   result = true;
    family field;

    uint64_t x_u64 = {};
    int64_t  x_64  = {};
    double   x_f   = {};
    u8string x_s;

    if (in.parse(" true ")) {
      field = true;
    } else if (in.parse(" false ")) {
      field = false;
    } else if (in.parse(" %D ", &x_u64)) {
      field = x_u64;
    } else if (in.parse(" %d ", &x_64)) {
      field = x_64;
    } else if (in.parse(" 0x%X ", &x_u64)) {
      field = x_u64;
    } else if (in.parse(" %f ", &x_f)) {
      field = x_f;
    } else if (in.parse(" \"%s\" ", &x_s)) {
      field = x_s;
    } else if (in.parse(" %a ", &x_s)) {
      auto id = family(x_s);

      if (in.parse(" ( ")) {
        field[text::s_function] = id;

        if (!in.parse(" ) ")) {
          family args;

          if (!parse(in, args)) {
            result = false;
          } else {
            if (args.is_vector()) {
              field[text::s_arguments] = args;
            } else {
              field[text::s_arguments][0] = args;
            }

            result = in.parse(" ) ");
          }
        }
      } else {
        field = id;
      }
    } else if (in.parse(" ( ")) {
      family v;

      if (!parse(in, v)) {
        result = false;
      } else {
        if (v.is_vector()) {
          field = v;
        } else {
          field[0] = v;
        }

        result = in.parse(" ) ");
      }
    } else if (in.parse(" { ")) {
      while (!in.parse(" } ")) {
        if (in.parse(" ; ")) {
          continue;
        }

        family key, value;

        if (!parse(in, key)) {
          result = false;
          break;
        }

        if (in.parse(" ( ")) {
          auto &cmd = field[text::s_commands];

          cmd[cmd.get_size()][text::s_function] = key;

          if (!in.parse(" ) ")) {
            if (!parse(in, value)) {
              result = false;
              break;
            }

            if (value.is_vector()) {
              cmd[cmd.get_size()][text::s_arguments] = value;
            } else {
              cmd[cmd.get_size()][text::s_arguments][0] = value;
            }

            result = in.parse(" ) ");
          }

          continue;
        }

        if ((!in.parse(" = ") && !in.parse(" : ")) ||
            !parse(in, value)) {
          result = false;
          break;
        }

        field[key] = value;
      }
    } else if (in.parse(" :: { ")) {
      vbyte v;

      while (!in.parse(" } ")) {
        if (uint64_t x; in.parse(" %X ", &x) && x <= 0xff) {
          v.emplace_back(static_cast<uint8_t>(x));
        } else {
          result = false;
          break;
        }
      }

      field = v;
    } else {
      result = false;
    }

    if (result) {
      if (f.is_vector()) {
        f[f.get_size()] = field;

        while (in.parse(" , ")) {
          if (!parse(in, f[f.get_size()])) {
            result = false;
            break;
          }
        }
      } else if (in.parse(" , ")) {
        f[0] = field;

        result = parse(in, f);
      } else {
        f = field;
      }
    }

    return result;
  }

  auto text::decode(fn_read read) -> pack_type {
    pack_type result;

    if (read) {
      parser in([read]() -> char32_t {
        auto c = read(1);
        if (c.size() == 1)
          return c[0];
        return 0;
      });

      result = make_shared<family>();

      if (!parse(in, *result)) {
        result.reset();
      }
    }

    return result;
  }

  static bool printdown(function<bool(const char *)> print,
                        const family &f, size_t indent = 0) {
    if (f.is_boolean()) {
      return print(f ? "true" : "false");
    } else if (f.is_integer()) {
      ostringstream out;
      out << f.get_integer();
      return print(out.str().c_str());
    } else if (f.is_uint()) {
      ostringstream out;
      out << f.get_uint();
      return print(out.str().c_str());
    } else if (f.is_real()) {
      ostringstream out;
      out << f.get_real();
      return print(out.str().c_str());
    } else if (f.is_string()) {
      auto v = string(as_ascii_string(f.get_string()));
      return print(v.c_str());
    } else if (f.is_bytes()) {
      if (!print(":: { "))
        return false;

      auto size = f.get_size();

      for (size_t i = 0; i < size; i++) {
        ostringstream out;
        out.width(2);
        out << hex;
        out << f.get_bytes()[i] << " ";
        print(out.str().c_str());
      }

      return print("}");
    } else if (f.is_vector()) {
      auto size = f.get_size();

      for (size_t i = 0; i < size; i++) {
        if (i > 0 && !print(", "))
          return false;
        if (!printdown(print, f[i], indent))
          return false;
      }
    } else if (f.is_composite()) {
      if (!print("{\n"))
        return false;

      auto size = f.get_size();

      for (size_t i = 0; i < size; i++) {
        for (size_t n = 0; n <= indent; n++)
          if (!print("  "))
            return false;
        const auto &k = f.get_key(i);
        if (!printdown(print, k, indent + 1) || !print(" = "))
          return false;
        if (!printdown(print, f[k], indent + 1) || !print("\n"))
          return false;
      }

      return print("}");
    }

    return print("{ }");
  }

  auto text::encode(fn_write write, const_pack_type data) -> bool {
    bool result = false;

    if (data) {
      auto print = [write](const char *s) -> bool {
        vbyte v(strlen(s));
        memcpy(v.data(), s, v.size());
        return write(v) == v.size();
      };

      result = printdown(print, data);
    }

    return result;
  }
}
