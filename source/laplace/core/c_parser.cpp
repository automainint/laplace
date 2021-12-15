/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "parser.h"

#include "utils.h"
#include <cmath>
#include <cstdarg>
#include <iostream>
#include <utility>

#ifdef _MSC_VER
/*  False va_arg warnings.
 */
#  pragma warning(disable : 6285)
#endif

namespace laplace::core {
  using std::u8string;

  parser::parser(parser::input_stream stream) noexcept :
      m_stream(std::move(stream)) { }

  void parser::push_offset() noexcept {
    if (!m_buffer_offset.empty()) {
      m_buffer_offset.emplace_back(m_buffer_offset.back());
    } else {
      error_("No buffered data.", __FUNCTION__);
    }
  }

  void parser::pop_offset(bool apply) noexcept {
    if (m_buffer_offset.size() >= 2) {
      if (!apply) {
        m_buffer_offset[m_buffer_offset.size() - 2] =
            m_buffer_offset.back();
      }

      m_buffer_offset.resize(m_buffer_offset.size() - 1);

      m_line   = m_buffer_offset.back().line;
      m_column = m_buffer_offset.back().column;

      m_is_eof = m_buffer_offset.back().offset >= m_buffer.size();

    } else {
      error_("No buffered data.", __FUNCTION__);
    }
  }

  auto parser::wrap(std::string_view s) noexcept -> parser {
    return parser([s, i = sl::index {}]() mutable -> char32_t {
      return i < s.size() ? s[i++] : U'\0';
    });
  }

  auto parser::wrap(std::u8string_view s) noexcept -> parser {
    return parser([s, i = sl::index {}]() mutable -> char32_t {
      return i < s.size() ? s[i++] : U'\0';
    });
  }

  void parser::apply(bool is_ok) noexcept {
    if (!m_buffer_offset.empty()) {
      if (is_ok) {
        if (m_buffer_offset.size() == 1) {
          m_buffer.erase(m_buffer.begin(),
                         m_buffer.begin() +
                             m_buffer_offset.back().offset);

          m_buffer_offset.back().offset = 0;
        }

        m_line   = m_buffer_offset.back().line;
        m_column = m_buffer_offset.back().column;

      } else {
        m_buffer_offset.back().offset = 0;
      }

    } else {
      error_("No buffered data.", __FUNCTION__);
    }
  }

  auto parser::get_char() noexcept -> char32_t {
    char32_t result = 0;

    if (!m_buffer_offset.empty()) {
      if (m_buffer_offset.back().offset < m_buffer.size()) {
        result = m_buffer[m_buffer_offset.back().offset];
        m_buffer_offset.back().offset++;
      } else {
        result = m_stream();

        if (result != 0) {
          if (result == '\n') {
            m_buffer_offset.back().column = 0;
            m_buffer_offset.back().line++;
          } else {
            m_buffer_offset.back().column++;
          }

          m_buffer.emplace_back(result);
          m_buffer_offset.back().offset++;
        } else {
          m_is_eof = true;
        }
      }
    } else {
      error_("No buffered data.", __FUNCTION__);
    }

    return result;
  }

  void parser::unget_char() noexcept {
    if (!m_buffer_offset.empty() &&
        m_buffer_offset.back().offset > 0) {
      m_buffer_offset.back().offset--;
    } else {
      error_("No buffered data.", __FUNCTION__);
    }
  }

  auto parser::is_path(char32_t c) noexcept -> bool {
    if (c < 32) {
      return false;
    }

    if (c == '*' || c == '?' || c == '"' || c == '<' || c == '>' ||
        c == '|' || c == '+') {
      return false;
    }

    if (c == '%' || c == '!' || c == '@') {
      return false;
    }

    return true;
  }

  auto parser::is_url(char32_t c) noexcept -> bool {
    if (c >= '0' && c <= '9') {
      return true;
    }

    if (c >= 'a' && c <= 'z') {
      return true;
    }

    if (c >= 'A' && c <= 'Z') {
      return true;
    }

    if (c == '-' || c == '_' || c == '.' || c == '~') {
      return true;
    }

    if (c == ':' || c == '@' || c == '/' || c == '?' || c == '&' ||
        c == '=' || c == '+' || c == '%') {
      return true;
    }

    return false;
  }

  auto parser::is_hex(char32_t c) noexcept -> bool {
    if (c >= '0' && c <= '9')
      return true;
    if (c >= 'a' && c <= 'f')
      return true;
    if (c >= 'A' && c <= 'F')
      return true;
    return false;
  }

  auto parser::string_end(char32_t c, const char *p) noexcept
      -> bool {
    if (p[1] == '%') {
      switch (p[2]) {
        case c_line_end: return c == '\r' || c == '\n';
        case c_bin_integer: return c == '-' || c == '0' || c == '1';
        case c_oct_integer: return c == '-' || (c >= '0' && c <= '7');
        case c_float:
        case c_dec_integer: return c == '-' || (c >= '0' && c <= '9');
        case c_hex_integer: return c == '-' || is_hex(c);
        case c_bin_uint: return c == '0' || c == '1';
        case c_oct_uint: return c >= '0' && c <= '7';
        case c_dec_uint: return c >= '0' && c <= '9';
        case c_hex_uint: return is_hex(c);
        case c_char:
        case c_string:
        case c_id_str:
        case c_word_str:
        case c_file_path:
        case c_url:
          error_("Invalid sequence.", __FUNCTION__);
          return true;
      }

      return c == static_cast<char32_t>(p[2]);
    }

    return c == static_cast<char32_t>(p[1]);
  }

  auto parser::parse(const char *format, ...) noexcept -> bool {
    va_list ap;
    va_start(ap, format);

    bool result    = true;
    bool is_done   = false;
    bool is_silent = false;

    auto const *p = format;

    while (!is_done) {
      if (*p == '\0') {
        is_done = true;
      } else if (m_is_eof) {
        for (; *p != '\0'; p++) {
          if (*p != ' ') {
            result = false;
            break;
          }
        }
        is_done = true;
      } else if (*p == '%') {
        is_silent = false;
        p++;

        if (*p == c_silent) {
          is_silent = true;
          p++;
        }

        if (*p == '\0') {
          result  = false;
          is_done = true;
        } else if (*p == '%') {
          if (get_char() != '%') {
            result  = false;
            is_done = true;
          }
        } else if (*p == c_whitespace) {
          /*  One whitespace.
           */

          if (get_char() != ' ') {
            result  = false;
            is_done = true;
          }
        } else if (*p == c_line_end) {
          /*  One line end.
           */

          char32_t c = get_char();

          if (c == '\r') {
            if (get_char() != '\n') {
              result  = false;
              is_done = true;
            }
          } else if (c != '\n') {
            result  = false;
            is_done = true;
          }
        } else if (*p == c_bin_integer) {
          /*  Binary integer.
           */

          int64_t i_value     = -1;
          bool    is_negative = false;

          char32_t c = get_char();

          if (c == '-') {
            is_negative = true;
            c           = get_char();
          } else if (c == '+') {
            c = get_char();
          }

          while (c == '0' || c == '1') {
            if (i_value == -1) {
              i_value = 0;
            } else {
              i_value *= 2;
            }

            i_value += static_cast<int64_t>(c) - '0';

            c = get_char();
          }

          unget_char();

          if (i_value == -1) {
            result  = false;
            is_done = true;
          } else if (!is_silent) {
            if (is_negative) {
              i_value = -i_value;
            }

            *va_arg(ap, int64_t *) = i_value;
          }
        } else if (*p == c_oct_integer) {
          /*  Oct integer.
           */

          int64_t i_value     = -1;
          bool    is_negative = false;

          char32_t c = get_char();

          if (c == '-') {
            is_negative = true;
            c           = get_char();
          } else if (c == '+') {
            c = get_char();
          }

          while (c >= '0' && c <= '7') {
            if (i_value == -1) {
              i_value = 0;
            } else {
              i_value *= 8;
            }

            i_value += static_cast<int64_t>(c) - '0';

            c = get_char();
          }

          unget_char();

          if (i_value == -1) {
            result  = false;
            is_done = true;
          } else if (!is_silent) {
            if (is_negative) {
              i_value = -i_value;
            }

            *va_arg(ap, int64_t *) = i_value;
          }
        } else if (*p == c_dec_integer) {
          /*  Decimal integer.
           */

          int64_t i_value     = -1;
          bool    is_negative = false;

          char32_t c = get_char();

          if (c == '-') {
            is_negative = true;
            c           = get_char();
          } else if (c == '+') {
            c = get_char();
          }

          while (c >= '0' && c <= '9') {
            if (i_value == -1) {
              i_value = 0;
            } else {
              i_value *= 10;
            }

            i_value += static_cast<int64_t>(c) - '0';

            c = get_char();
          }

          unget_char();

          if (i_value == -1) {
            result  = false;
            is_done = true;
          } else if (!is_silent) {
            if (is_negative) {
              i_value = -i_value;
            }

            *va_arg(ap, int64_t *) = i_value;
          }
        } else if (*p == c_hex_integer) {
          /*  Hex integer.
           */

          int64_t i_value     = -1;
          bool    is_negative = false;

          char32_t c = get_char();

          if (c == '-') {
            is_negative = true;
            c           = get_char();
          } else if (c == '+') {
            c = get_char();
          }

          while (is_hex(c)) {
            if (i_value == -1) {
              i_value = 0;
            } else {
              i_value *= 16;
            }

            i_value += hex_to_int(c);

            c = get_char();
          }

          unget_char();

          if (i_value == -1) {
            result  = false;
            is_done = true;
          } else if (!is_silent) {
            if (is_negative) {
              i_value = -i_value;
            }

            *va_arg(ap, int64_t *) = i_value;
          }
        } else if (*p == c_bin_uint) {
          /*  Binary uint.
           */

          uint64_t i_value = -1;
          bool     is_set  = false;

          char32_t c = get_char();

          while (c == '0' || c == '1') {
            if (!is_set) {
              is_set  = true;
              i_value = 0;
            } else {
              i_value *= 2;
            }

            i_value += static_cast<int64_t>(c) - '0';

            c = get_char();
          }

          unget_char();

          if (!is_set) {
            result  = false;
            is_done = true;
          } else if (!is_silent) {
            *va_arg(ap, uint64_t *) = i_value;
          }
        } else if (*p == c_oct_uint) {
          /*  Oct uint.
           */

          uint64_t i_value = -1;
          bool     is_set  = false;

          char32_t c = get_char();

          while (c >= '0' && c <= '7') {
            if (!is_set) {
              is_set  = true;
              i_value = 0;
            } else {
              i_value *= 8;
            }

            i_value += static_cast<int64_t>(c) - '0';

            c = get_char();
          }

          unget_char();

          if (!is_set) {
            result  = false;
            is_done = true;
          } else if (!is_silent) {
            *va_arg(ap, uint64_t *) = i_value;
          }
        } else if (*p == c_dec_uint) {
          /*  Decimal uint.
           */

          uint64_t i_value = -1;
          bool     is_set  = false;

          char32_t c = get_char();

          while (c >= '0' && c <= '9') {
            if (!is_set) {
              is_set  = true;
              i_value = 0;
            } else {
              i_value *= 10;
            }

            i_value += static_cast<int64_t>(c) - '0';

            c = get_char();
          }

          unget_char();

          if (!is_set) {
            result  = false;
            is_done = true;
          } else if (!is_silent) {
            *va_arg(ap, uint64_t *) = i_value;
          }
        } else if (*p == c_hex_uint) {
          /*  Hex uint.
           */

          uint64_t i_value = -1;
          bool     is_set  = false;

          char32_t c = get_char();

          while (is_hex(c)) {
            if (!is_set) {
              is_set  = true;
              i_value = 0;
            } else {
              i_value *= 16;
            }

            i_value += hex_to_int(c);

            c = get_char();
          }

          unget_char();

          if (!is_set) {
            result  = false;
            is_done = true;
          } else if (!is_silent) {
            *va_arg(ap, uint64_t *) = i_value;
          }
        } else if (*p == c_float) {
          /*  Floating-point number.
           */

          bool    f_negative     = false;
          int64_t f_whole        = -1;
          int64_t f_frac_value   = -1;
          int64_t f_frac_power   = 0;
          bool    f_exp_negative = false;
          int64_t f_exp_value    = -1;

          int64_t c = get_char();

          if (c == '-') {
            f_negative = true;
            c          = get_char();
          } else if (c == '+') {
            c = get_char();
          }

          while (c >= '0' && c <= '9') {
            if (f_whole == -1) {
              f_whole = 0;
            } else {
              f_whole *= 10;
            }

            f_whole += c - '0';

            c = get_char();
          }

          if (c == '.') {
            c = get_char();

            while (c >= '0' && c <= '9') {
              if (f_frac_value == -1) {
                f_frac_value = 0;
              } else {
                f_frac_value *= 10;
              }

              f_frac_value += c - '0';
              f_frac_power++;

              c = get_char();
            }
          }

          if (c == 'e' || c == 'E') {
            c = get_char();

            if (c == '-') {
              f_exp_negative = true;
              c              = get_char();
            } else if (c == '+') {
              c = get_char();
            }

            while (c >= '0' && c <= '9') {
              if (f_exp_value == -1) {
                f_exp_value = 0;
              } else {
                f_exp_value *= 10;
              }

              f_exp_value += c - '0';

              c = get_char();
            }

            if (f_exp_value == -1) {
              result  = false;
              is_done = true;
            }
          }

          unget_char();

          if (!is_done) {
            if (f_whole == -1 && f_frac_value == -1) {
              result  = false;
              is_done = true;
            } else if (!is_silent) {
              long double f_value = {};

              if (f_whole != -1) {
                f_value = static_cast<decltype(f_value)>(f_whole);
              }

              if (f_frac_value != -1) {
                f_value += static_cast<decltype(f_value)>(
                               f_frac_value) *
                           pow(10,
                               static_cast<double>(-f_frac_power));
              }

              if (f_exp_value != -1) {
                if (f_exp_negative) {
                  f_exp_value = -f_exp_value;
                }

                f_value *= pow(10, static_cast<double>(f_exp_value));
              }

              if (f_negative) {
                f_value = -f_value;
              }

              *va_arg(ap, double *) = static_cast<double>(f_value);
            }
          }
        } else if (*p == c_char) {
          /*  One char.
           */

          char32_t c = get_char();

          if (c == '\0') {
            result  = false;
            is_done = true;
          } else if (!is_silent) {
            *va_arg(ap, char32_t *) = c;
          }
        } else if (*p == c_string) {
          /*  String.
           */

          auto s_value = u8string {};

          bool is_empty = true;
          auto c        = get_char();

          while (!string_end(c, p)) {
            is_empty = false;

            if (c == '\\') {
              if (!is_silent) {
                s_value.append(1, static_cast<char8_t>(c));
              }

              c = get_char();
            }

            if (!is_silent) {
              s_value.append(1, static_cast<char8_t>(c));
            }

            if (c == '\0') {
              result  = false;
              is_done = true;
            }

            c = get_char();
          }

          unget_char();

          if (is_empty) {
            result  = false;
            is_done = true;
          } else if (!is_done && !is_silent) {
            *va_arg(ap, u8string *) = s_value;
          }
        } else if (*p == c_id_str) {
          /*  Id string.
           */

          u8string s_value;

          char32_t c = get_char();

          if (c == '_' || (c >= 'a' && c <= 'z') ||
              (c >= 'A' && c <= 'Z')) {
            if (!is_silent) {
              s_value.append(1, static_cast<char8_t>(c));
            }

            c = get_char();

            while (c == '_' || (c >= '0' && c <= '9') ||
                   (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
              if (!is_silent) {
                s_value.append(1, static_cast<char8_t>(c));
              }

              c = get_char();
            }

            unget_char();

            if (!is_silent) {
              *va_arg(ap, u8string *) = s_value;
            }
          } else {
            result  = false;
            is_done = true;
          }
        } else if (*p == c_word_str) {
          /*  Word string.
           */

          u8string s_value;

          bool     is_empty = true;
          char32_t c        = get_char();

          while ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            is_empty = false;

            if (!is_silent) {
              s_value.append(1, static_cast<char8_t>(c));
            }

            c = get_char();
          }

          unget_char();

          if (is_empty) {
            result  = false;
            is_done = true;
          } else if (!is_silent) {
            *va_arg(ap, u8string *) = s_value;
          }
        } else if (*p == c_file_path) {
          /*  File path string.
           */

          u8string s_value;

          bool     is_empty = true;
          char32_t c        = get_char();

          while (is_path(c)) {
            is_empty = false;

            if (!is_silent) {
              s_value.append(1, static_cast<char8_t>(c));
            }

            c = get_char();
          }

          unget_char();

          if (is_empty) {
            result  = false;
            is_done = true;
          } else if (!is_silent) {
            *va_arg(ap, u8string *) = s_value;
          }
        } else if (*p == c_url) {
          /*  URL string.
           */

          u8string s_value;

          bool     is_empty = true;
          char32_t c        = get_char();

          while (is_url(c)) {
            is_empty = false;

            if (!is_silent) {
              s_value.append(1, static_cast<char8_t>(c));
            }

            c = get_char();
          }

          unget_char();

          if (is_empty) {
            result  = false;
            is_done = true;
          } else if (!is_silent) {
            *va_arg(ap, u8string *) = s_value;
          }
        }
      } else if (*p == ' ') {
        /*  Whitespaces.
         */

        char32_t c = get_char();

        while (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
          c = get_char();
        }

        unget_char();
      } else if (*p == '\n') {
        /*  Whitespaces until next line.
         */

        char32_t c = get_char();

        while (c == ' ' || c == '\t' || c == '\r') { c = get_char(); }

        if (c != '\n') {
          result  = false;
          is_done = true;
        }
      } else if (static_cast<char32_t>(*p) != get_char()) {
        result  = false;
        is_done = true;
      }

      p++;
    }

    va_end(ap);

    apply(result);
    return result;
  }

  void parser::set_stream(parser::input_stream stream) noexcept {
    this->m_stream = std::move(stream);
  }

  auto parser::get_stream() const noexcept -> parser::input_stream {
    return this->m_stream;
  }

  auto parser::is_eof() const noexcept -> bool {
    return m_is_eof;
  }

  auto parser::get_line() const noexcept -> sl::index {
    return m_line;
  }

  auto parser::get_column() const noexcept -> sl::index {
    return m_column;
  }
}
