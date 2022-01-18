/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef laplace_core_parser_h
#define laplace_core_parser_h

#include "slib.h"
#include <functional>

namespace laplace::core {
  class parser {
  public:
    using input_stream = std::function<char32_t(void)>;

    parser(parser &) = delete;
    auto operator=(parser &) -> parser & = delete;

    parser(parser &&) noexcept = default;
    auto operator=(parser &&) noexcept -> parser & = default;

    explicit parser(input_stream stream = nullptr) noexcept;
    ~parser() = default;

    /*  "% "    - one whitespace
     *  "%n"    - one line end
     *  "%b"    - bin integer   - int64_t *
     *  "%o"    - oct integer   - int64_t *
     *  "%d"    - dec integer   - int64_t *
     *  "%x"    - hex integer   - int64_t *
     *  "%B"    - bin uint      - uint64_t *
     *  "%O"    - oct uint      - uint64_t *
     *  "%D"    - dec uint      - uint64_t *
     *  "%X"    - hex uint      - uint64_t *
     *  "%f"    - float         - double *
     *  "%c"    - one char      - char32_t *
     *  "%s"    - string        - u8string *
     *  "%a"    - id string     - u8string *
     *  "%A"    - word string   - u8string *
     *  "%p"    - file path     - u8string *
     *  "%u"    - URL           - u8string *
     */
    auto parse(char const *format, ...) noexcept -> bool;

    void               set_stream(input_stream stream) noexcept;
    [[nodiscard]] auto get_stream() const noexcept -> input_stream;

    [[nodiscard]] auto is_eof() const noexcept -> bool;

    [[nodiscard]] auto get_line() const noexcept -> sl::index;
    [[nodiscard]] auto get_column() const noexcept -> sl::index;

    void push_offset() noexcept;

    /*  apply - if true, reverts saved offset.
     */
    void pop_offset(bool apply) noexcept;

    [[nodiscard]] static auto wrap(std::string_view s) noexcept
        -> parser;
    [[nodiscard]] static auto wrap(std::u8string_view s) noexcept
        -> parser;

  private:
    enum control_chars : char {
      c_silent      = '~',
      c_whitespace  = ' ',
      c_line_end    = 'n',
      c_bin_integer = 'b',
      c_oct_integer = 'o',
      c_dec_integer = 'd',
      c_hex_integer = 'x',
      c_bin_uint    = 'B',
      c_oct_uint    = 'O',
      c_dec_uint    = 'D',
      c_hex_uint    = 'X',
      c_float       = 'f',
      c_char        = 'c',
      c_string      = 's',
      c_id_str      = 'a',
      c_word_str    = 'A',
      c_file_path   = 'p',
      c_url         = 'u'
    };

    void apply(bool is_ok) noexcept;
    auto get_char() noexcept -> char32_t;
    void unget_char() noexcept;

    static auto is_path(char32_t c) noexcept -> bool;
    static auto is_url(char32_t c) noexcept -> bool;
    static auto is_hex(char32_t c) noexcept -> bool;

    static auto string_end(char32_t c, const char *p) noexcept
        -> bool;

    struct position {
      sl::index line   = 0;
      sl::index column = 0;
      sl::index offset = 0;
    };

    std::vector<char32_t> m_buffer;
    std::vector<position> m_buffer_offset = std::vector<position>(1);

    sl::index    m_line   = 0;
    sl::index    m_column = 0;
    bool         m_is_eof = false;
    input_stream m_stream;
  };
}

#endif
