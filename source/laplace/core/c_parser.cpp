/*  laplace/core/c_parser.cpp
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

#include "parser.h"
#include "utils.h"
#include <cstdarg>
#include <iostream>

#ifdef _MSC_VER
/*  False va_arg warnings.
 */
#  pragma warning(disable : 6285)
#endif

namespace laplace::core {
  using namespace std;

  parser::parser(parser::input_stream stream) {
    m_stream = stream;
  }

  void parser::push_offset() {
    if (!m_buffer_offset.empty()) {
      m_buffer_offset.emplace_back(m_buffer_offset.back());
    } else {
      error(__FUNCTION__, "No buffered data.");
    }
  }

  void parser::pop_offset(bool apply) {
    if (m_buffer_offset.size() >= 2) {
      if (apply) {
        m_buffer_offset[m_buffer_offset.size() - 2] =
            m_buffer_offset.back();
      }

      m_buffer_offset.resize(m_buffer_offset.size() - 1);

      m_line   = m_buffer_offset.back().line;
      m_column = m_buffer_offset.back().column;
    } else {
      error(__FUNCTION__, "No buffered data.");
    }
  }

  auto parser::wrap(std::string_view s) -> parser {
    return parser([s, i = size_t {}]() mutable -> char32_t {
      return i < s.size() ? s[i++] : 0;
    });
  }

  auto parser::wrap(std::u8string_view s) -> parser {
    return parser([s, i = size_t {}]() mutable -> char32_t {
      return i < s.size() ? s[i++] : 0;
    });
  }

  void parser::apply(bool is_ok) {
    if (!m_buffer_offset.empty()) {
      if (is_ok && m_buffer_offset.size() == 1) {
        m_buffer.clear();

        m_line   = m_buffer_offset.back().line;
        m_column = m_buffer_offset.back().column;
      }

      m_buffer_offset.back().offset = 0;
    } else {
      error(__FUNCTION__, "No buffered data.");
    }
  }

  auto parser::get_char() -> char32_t {
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
      error(__FUNCTION__, "No buffered data.");
    }

    return result;
  }

  void parser::unget_char() {
    if (!m_buffer_offset.empty() && m_buffer_offset.back().offset != 0) {
      m_buffer_offset.back().offset--;
    } else {
      error(__FUNCTION__, "No buffered data.");
    }
  }

  auto parser::is_path(char32_t c) -> bool {
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

  auto parser::is_url(char32_t c) -> bool {
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

  auto parser::is_hex(char32_t c) -> bool {
    bool result = false;

    if (c >= '0' && c <= '9') {
      result = true;
    } else if (c >= 'a' && c <= 'f') {
      result = true;
    } else if (c >= 'A' && c <= 'F') {
      result = true;
    }

    return result;
  }

  auto parser::string_end(char32_t c, const char *p) -> bool {
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
        case c_url: //
          verb("Parser: Invalid sequence.");
          return true;
      }

      return c == p[2];
    }

    return c == p[1];
  }

  auto parser::parse(const char *format, ...) -> bool {
    va_list ap;
    va_start(ap, format);

    bool result    = true;
    bool is_done   = false;
    bool is_silent = false;

    const char *p = format;

    while (!is_done) {
      if (*p == '\0') {
        is_done = true;
      } else if (m_is_eof) {
        for (; *p; p++) {
          if (*p != ' ') {
            result = false;
            break;
          }
        }
        is_done = true;
      } else if (*p == '%') {
        is_silent = false;
        p++;

        if (*p == '~') {
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
              i_value <<= 1;
            }

            i_value |= static_cast<int64_t>(c) - '0';

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
              i_value <<= 3;
            }

            i_value |= static_cast<int64_t>(c) - '0';

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
              i_value <<= 4;
            }

            i_value |= hex_to_int(c);

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
        } else if (*p == 'B') {
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
              i_value <<= 1;
            }

            i_value |= static_cast<int64_t>(c) - '0';

            c = get_char();
          }

          unget_char();

          if (!is_set) {
            result  = false;
            is_done = true;
          } else if (!is_silent) {
            *va_arg(ap, uint64_t *) = i_value;
          }
        } else if (*p == 'O') {
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
              i_value <<= 3;
            }

            i_value |= static_cast<int64_t>(c) - '0';

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
              i_value <<= 4;
            }

            i_value |= hex_to_int(c);

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

          bool f_negative     = false;
          int  f_whole        = -1;
          int  f_frac_value   = -1;
          int  f_frac_power   = 0;
          bool f_exp_negative = false;
          int  f_exp_value    = -1;

          char32_t c = get_char();

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
              long double f_value = 0;

              if (f_whole != -1) {
                f_value = f_whole;
              }

              if (f_frac_value != -1) {
                f_value += f_frac_value * pow(10, -f_frac_power);
              }

              if (f_exp_value != -1) {
                if (f_exp_negative) {
                  f_exp_value = -f_exp_value;
                }

                f_value *= pow(10, f_exp_value);
              }

              if (f_negative) {
                f_value = -f_value;
              }

              *va_arg(ap, double *) = f_value;
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

          u8string s_value;

          bool     is_empty = true;
          char32_t c        = get_char();

          while (!string_end(c, p)) {
            is_empty = false;

            if (c == '\\') {
              if (!is_silent) {
                s_value.append(1, c);
              }

              c = get_char();
            }

            if (!is_silent) {
              s_value.append(1, c);
            }

            if (c == '\0') {
              result  = false;
              is_done = true;
            }

            c = get_char();
          }

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
              s_value.append(1, c);
            }

            c = get_char();

            while (c == '_' || (c >= '0' && c <= '9') ||
                   (c >= 'a' && c <= 'z') ||
                   (c >= 'A' && c <= 'Z')) {
              if (!is_silent) {
                s_value.append(1, c);
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

          while ((c >= 'a' && c <= 'z') ||
                 (c >= 'A' && c <= 'Z')) {
            is_empty = false;

            if (!is_silent) {
              s_value.append(1, c);
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
              s_value.append(1, c);
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
              s_value.append(1, c);
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
      } else if (*p != get_char()) {
        result  = false;
        is_done = true;
      }

      p++;
    }

    va_end(ap);

    apply(result);
    return result;
  }

  void parser::set_stream(parser::input_stream stream) {
    this->m_stream = stream;
  }

  auto parser::get_stream() const -> parser::input_stream {
    return this->m_stream;
  }

  auto parser::is_eof() const -> bool {
    return m_is_eof;
  }

  auto parser::get_line() const -> size_t {
    return m_line;
  }

  auto parser::get_column() const -> size_t {
    return m_column;
  }
}
