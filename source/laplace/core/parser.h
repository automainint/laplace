#pragma once

#include <functional>
#include <string>
#include <cstdint>

namespace laplace::core
{
    class parser
    {
    public:
        using input_stream = std::function<char32_t (void)>;

        parser(parser &&) = default;
        auto operator =(parser &&) -> parser & = default;

        parser(input_stream stream = nullptr);

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
        auto parse(const char *format, ...) -> bool;

        void set_stream(input_stream stream);

        auto get_stream() const -> input_stream;

        auto is_eof() const -> bool;

        auto get_line() const -> size_t;
        auto get_column() const -> size_t;

        void push_offset();
        void pop_offset(bool apply);

        static auto wrap(std::string_view s, size_t i = 0) -> parser;
        static auto wrap(std::u8string_view s, size_t i = 0) -> parser;

    private:
        void apply(bool is_ok);
        auto get_char() -> char32_t;
        void unget_char();

        static auto is_path(char32_t c) -> bool;
        static auto is_url(char32_t c) -> bool;
        static auto is_hex(char32_t c) -> bool;

        struct position
        {
            size_t line     = 0;
            size_t column   = 0;
            size_t offset   = 0;
        };

        std::vector<char32_t> m_buffer;
        std::vector<position> m_buffer_offset = std::vector<position>(1);

        size_t          m_line      = 0;
        size_t          m_column    = 0;
        bool            m_is_eof    = false;
        input_stream    m_stream;
    };
}
