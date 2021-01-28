#pragma once

#include "defs.h"
#include <memory>
#include <variant>

namespace laplace::core
{
    /*  Indexed family implementation.
     *
     *  Universal data container.
     */
    class family
    {
    public:
        using ref_family    = family &;
        using cref_family   = const family &;

        using vfamily   = std::vector<family>;
        using composite = std::vector<std::pair<family, family>>;

        using cref_vfamily      = const vfamily &;
        using cref_composite    = const composite &;

        family();
        ~family();

        family(cref_family) = default;
        family(family &&) = default;

        auto operator =(cref_family) -> ref_family = default;
        auto operator =(family &&) -> ref_family = default;

        family(cref_vfamily value);
        family(cref_composite value);
        family(bool value);
        family(int8_t value);
        family(int16_t value);
        family(int32_t value);
        family(int64_t value);
        family(uint8_t value);
        family(uint16_t value);
        family(uint32_t value);
        family(uint64_t value);
        family(float value);
        family(double value);
        family(std::string_view value);
        family(std::wstring_view value);
        family(std::u8string_view value);
        family(std::u16string_view value);
        family(std::u32string_view value);
        family(const char *value);
        family(const wchar_t *value);
        family(const char8_t *value);
        family(const char16_t *value);
        family(const char32_t *value);
        family(cref_vbyte value);

        auto operator =(cref_vfamily value) -> ref_family;
        auto operator =(cref_composite value) -> ref_family;
        auto operator =(bool value) -> ref_family;
        auto operator =(int8_t value) -> ref_family;
        auto operator =(int16_t value) -> ref_family;
        auto operator =(int32_t value) -> ref_family;
        auto operator =(int64_t value) -> ref_family;
        auto operator =(uint8_t value) -> ref_family;
        auto operator =(uint16_t value) -> ref_family;
        auto operator =(uint32_t value) -> ref_family;
        auto operator =(uint64_t value) -> ref_family;
        auto operator =(float value) -> ref_family;
        auto operator =(double value) -> ref_family;
        auto operator =(std::string_view value) -> ref_family;
        auto operator =(std::wstring_view value) -> ref_family;
        auto operator =(std::u8string_view value) -> ref_family;
        auto operator =(std::u16string_view value) -> ref_family;
        auto operator =(std::u32string_view value) -> ref_family;
        auto operator =(const char *value) -> ref_family;
        auto operator =(const wchar_t *value) -> ref_family;
        auto operator =(const char8_t *value)->ref_family;
        auto operator =(const char16_t *value)->ref_family;
        auto operator =(const char32_t *value) -> ref_family;
        auto operator =(cref_vbyte value) -> ref_family;

        auto operator ==(cref_family value) const -> bool;
        auto operator !=(cref_family value) const -> bool;
        auto operator <(cref_family value) const -> bool;
        auto operator >(cref_family value) const -> bool;
        auto operator <=(cref_family value) const -> bool;
        auto operator >=(cref_family value) const -> bool;

        operator bool() const;
        operator int64_t() const;
        operator double() const;
        operator uint64_t() const;
        operator std::u8string_view() const;

        auto operator [](size_t index) -> ref_family;
        auto operator [](size_t index) const -> cref_family;

        auto operator [](cref_family key) -> ref_family;
        auto operator [](cref_family key) const -> cref_family;

        auto is_empty() const -> bool;
        auto is_boolean() const -> bool;
        auto is_integer() const -> bool;
        auto is_uint() const -> bool;
        auto is_real() const -> bool;
        auto is_string() const -> bool;
        auto is_bytes() const -> bool;
        auto is_vector() const -> bool;
        auto is_composite() const -> bool;

        auto get_boolean() const -> bool;
        auto get_integer() const -> int64_t;
        auto get_real() const -> double;
        auto get_string() const -> std::u8string_view;
        auto get_uint() const -> uint64_t;
        auto get_bytes() const -> cref_vbyte;

        auto get_size() const -> size_t;
        auto has(cref_family key) const -> bool;

        /*  Keys are sorted. Setting a key
         *  will change the keys order.
         */
        void set_key(size_t index, cref_family k);

        /*  Create a new key if needed.
         */
        void key(cref_family k);

        auto value(size_t index) -> ref_family;
        auto value(cref_family key) -> ref_family;

        auto get_key(size_t index) const -> cref_family;
        auto get_value(size_t index) const -> cref_family;
        auto get_value(cref_family key) const -> cref_family;

        auto by_key(size_t key) -> ref_family;
        auto by_key(size_t key) const -> cref_family;

        auto compare(cref_family value) const -> int;

        auto value(int index) -> ref_family;
        auto value(const char *key) -> ref_family;

        auto get_value(int index) const -> cref_family;
        auto get_value(const char *key) const -> cref_family;

        /*  Merge composite data. Returns false
         *  if any field has been overwritten.
         */
        auto merge(cref_family f) -> bool;

        operator float() const;
        operator int32_t() const;
        operator uint32_t() const;

        auto operator [](int index) -> ref_family;
        auto operator [](int index) const -> cref_family;

        auto operator [](const char *key) -> ref_family;
        auto operator [](const char *key) const -> cref_family;

        auto operator [](const char8_t *key) -> ref_family;
        auto operator [](const char8_t *key) const -> cref_family;

        static auto logic_error() -> cref_family;
        static auto out_of_range() -> cref_family;

    private:
        void assign(cref_vfamily value);
        void assign(cref_composite value);
        void assign(bool value);
        void assign(int64_t value);
        void assign(uint64_t value);
        void assign(double value);
        void assign(std::string_view value);
        void assign(std::wstring_view value);
        void assign(std::u8string_view value);
        void assign(std::u16string_view value);
        void assign(std::u32string_view value);
        void assign(cref_vbyte value);

        static auto op_lower_bound(const std::pair<family, family> &a, const family &key) -> bool;
        static auto op_lower_bound_uint(const std::pair<family, family> &a, size_t key) -> bool;
        static auto op_sort(const std::pair<family, family> &a, const std::pair<family, family> &b) -> bool;

        enum type_index : size_t
        {
            n_uint = 1,
            n_bool,
            n_int,
            n_real,
            n_string,
            n_bytes,
            n_vector,
            n_composite
        };

        using field_type = std::variant<
            std::monostate,
            uint64_t, bool, int64_t, double,
            std::u8string, vbyte, vfamily, composite
        >;

        field_type m_data;
    };

    using ref_family    = family::ref_family;
    using cref_family   = family::cref_family;
    using vfamily       = family::vfamily;
    using composite     = family::composite;

    using ptr_family = std::shared_ptr<family>;
}

#include "family.impl.h"
