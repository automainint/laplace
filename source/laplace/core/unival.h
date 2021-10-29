/*  laplace/core/unival.h
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

#ifndef laplace_core_unival_h
#define laplace_core_unival_h

#include "defs.h"

namespace laplace::core {
  class unival {
  public:
    using ref_unival  = unival &;
    using cref_unival = unival const &;

    using vec       = sl::vector<unival>;
    using composite = sl::vector<std::pair<unival, unival>>;

    using cref_vec       = vec const &;
    using cref_composite = composite const &;

    unival() noexcept  = default;
    ~unival() noexcept = default;

    unival(cref_unival) noexcept = default;
    unival(unival &&) noexcept   = default;

    auto operator=(cref_unival) noexcept -> unival & = default;
    auto operator=(unival &&) noexcept -> unival & = default;

    /*  Create array.
     */
    unival(cref_vec value) noexcept;

    /*  Create composite.
     */
    unival(cref_composite value) noexcept;

    unival(bool value) noexcept;
    unival(signed char value) noexcept;
    unival(signed short value) noexcept;
    unival(signed int value) noexcept;
    unival(signed long value) noexcept;
    unival(signed long long value) noexcept;
    unival(unsigned char value) noexcept;
    unival(unsigned short value) noexcept;
    unival(unsigned int value) noexcept;
    unival(unsigned long value) noexcept;
    unival(unsigned long long value) noexcept;
    unival(float value) noexcept;
    unival(double value) noexcept;
    unival(std::string_view value) noexcept;
    unival(std::wstring_view value) noexcept;
    unival(std::u8string_view value) noexcept;
    unival(std::u16string_view value) noexcept;
    unival(std::u32string_view value) noexcept;
    unival(const char *value) noexcept;
    unival(const wchar_t *value) noexcept;
    unival(const char8_t *value) noexcept;
    unival(const char16_t *value) noexcept;
    unival(const char32_t *value) noexcept;
    unival(span_cbyte value) noexcept;

    auto operator=(cref_vec value) noexcept -> ref_unival;
    auto operator=(cref_composite value) noexcept -> ref_unival;
    auto operator=(bool value) noexcept -> ref_unival;
    auto operator=(signed char value) noexcept -> ref_unival;
    auto operator=(signed short value) noexcept -> ref_unival;
    auto operator=(signed int value) noexcept -> ref_unival;
    auto operator=(signed long value) noexcept -> ref_unival;
    auto operator=(signed long long value) noexcept -> ref_unival;
    auto operator=(unsigned char value) noexcept -> ref_unival;
    auto operator=(unsigned short value) noexcept -> ref_unival;
    auto operator=(unsigned int value) noexcept -> ref_unival;
    auto operator=(unsigned long value) noexcept -> ref_unival;
    auto operator=(unsigned long long value) noexcept -> ref_unival;
    auto operator=(float value) noexcept -> ref_unival;
    auto operator=(double value) noexcept -> ref_unival;
    auto operator=(std::string_view value) noexcept -> ref_unival;
    auto operator=(std::wstring_view value) noexcept -> ref_unival;
    auto operator=(std::u8string_view value) noexcept -> ref_unival;
    auto operator=(std::u16string_view value) noexcept -> ref_unival;
    auto operator=(std::u32string_view value) noexcept -> ref_unival;
    auto operator=(const char *value) noexcept -> ref_unival;
    auto operator=(const wchar_t *value) noexcept -> ref_unival;
    auto operator=(const char8_t *value) noexcept -> ref_unival;
    auto operator=(const char16_t *value) noexcept -> ref_unival;
    auto operator=(const char32_t *value) noexcept -> ref_unival;
    auto operator=(span_cbyte value) noexcept -> ref_unival;

    [[nodiscard]] auto operator==(cref_unival value) const noexcept
        -> bool;
    [[nodiscard]] auto operator!=(cref_unival value) const noexcept
        -> bool;
    [[nodiscard]] auto operator<(cref_unival value) const noexcept
        -> bool;
    [[nodiscard]] auto operator>(cref_unival value) const noexcept
        -> bool;
    [[nodiscard]] auto operator<=(cref_unival value) const noexcept
        -> bool;
    [[nodiscard]] auto operator>=(cref_unival value) const noexcept
        -> bool;

    operator bool() const noexcept;
    operator signed long long() const noexcept;
    operator unsigned long long() const noexcept;
    operator double() const noexcept;

    auto operator[](signed long long n) noexcept -> ref_unival;
    auto operator[](signed long long n) const noexcept -> cref_unival;

    auto operator[](cref_unival key) noexcept -> ref_unival;
    auto operator[](cref_unival key) const noexcept -> cref_unival;

    /*  Returns true if empty.
     */
    [[nodiscard]] auto is_empty() const noexcept -> bool;

    /*  Returns true if is boolean.
     */
    [[nodiscard]] auto is_boolean() const noexcept -> bool;

    /*  Returns true if is integer.
     */
    [[nodiscard]] auto is_integer() const noexcept -> bool;

    /*  Returns true if is unsigned integer.
     */
    [[nodiscard]] auto is_uint() const noexcept -> bool;

    /*  Returns true if is floating-point number.
     */
    [[nodiscard]] auto is_real() const noexcept -> bool;

    /*  Returns true if is string.
     */
    [[nodiscard]] auto is_string() const noexcept -> bool;

    /*  Returns true if is byte array.
     */
    [[nodiscard]] auto is_bytes() const noexcept -> bool;

    /*  Returns true if is vector.
     */
    [[nodiscard]] auto is_vector() const noexcept -> bool;

    /*  Returns true if is composite.
     */
    [[nodiscard]] auto is_composite() const noexcept -> bool;

    /*  Returns boolean value.
     */
    [[nodiscard]] auto get_boolean() const noexcept -> bool;

    /*  Returns integer value.
     */
    [[nodiscard]] auto get_integer() const noexcept
        -> signed long long;

    /*  Returns unsigned integer value.
     */
    [[nodiscard]] auto get_uint() const noexcept
        -> unsigned long long;

    /*  Returns floating-point number value.
     */
    [[nodiscard]] auto get_real() const noexcept -> double;

    /*  Returns string value.
     */
    [[nodiscard]] auto get_string() const noexcept
        -> std::u8string_view;

    /*  Returns byte array.
     */
    [[nodiscard]] auto get_bytes() const noexcept -> span_cbyte;

    /*  Return vector or composite size in elements.
     */
    [[nodiscard]] auto get_size() const noexcept -> sl::whole;

    /*  Return true if composite has element by key.
     */
    [[nodiscard]] auto has(cref_unival key) const noexcept -> bool;

    /*  Set composite key by index.
     *
     *  Keys are sorted. Setting a key will change the keys order.
     */
    void set_key(signed long long n, cref_unival k) noexcept;

    /*  Create a new key if needed.
     */
    void key(cref_unival k) noexcept;

    /*  Get vector element by index.
     */
    auto value(signed long long n) noexcept -> ref_unival;

    /*  Get composite element by key.
     */
    auto value(cref_unival key) noexcept -> ref_unival;

    /*  Get composite key by index.
     */
    [[nodiscard]] auto get_key(signed long long n) const noexcept
        -> cref_unival;

    /*  Get vector element by index.
     */
    [[nodiscard]] auto get_value(signed long long n) const noexcept
        -> cref_unival;

    /*  Get composite element by key.
     */
    [[nodiscard]] auto get_value(cref_unival key) const noexcept
        -> cref_unival;

    /*  Get composite element by key.
     */
    [[nodiscard]] auto by_key(signed long long key) noexcept
        -> ref_unival;

    /*  Get composite element by key.
     */
    [[nodiscard]] auto by_key(signed long long key) const noexcept
        -> cref_unival;

    /*  Get composite element by index.
     */
    [[nodiscard]] auto by_index(signed long long n) noexcept
        -> ref_unival;

    /*  Get composite element by index.
     */
    [[nodiscard]] auto by_index(signed long long n) const noexcept
        -> cref_unival;

    /*  Returns -1 if less, +1 if greater, 0 if equals.
     */
    [[nodiscard]] auto compare(cref_unival value) const noexcept
        -> int;

    /*  Merge composite data. Returns false
     *  if any field has been overwritten.
     */
    auto merge(cref_unival f) noexcept -> bool;

    auto value(signed int n) noexcept -> ref_unival;
    auto value(signed long n) noexcept -> ref_unival;
    auto value(unsigned int n) noexcept -> ref_unival;
    auto value(unsigned long n) noexcept -> ref_unival;
    auto value(unsigned long long n) noexcept -> ref_unival;

    auto value(const char *key) noexcept -> ref_unival;
    auto value(const char8_t *key) noexcept -> ref_unival;

    [[nodiscard]] auto get_key(signed int n) const noexcept
        -> cref_unival;
    [[nodiscard]] auto get_key(signed long n) const noexcept
        -> cref_unival;
    [[nodiscard]] auto get_key(unsigned int n) const noexcept
        -> cref_unival;
    [[nodiscard]] auto get_key(unsigned long n) const noexcept
        -> cref_unival;
    [[nodiscard]] auto get_key(unsigned long long n) const noexcept
        -> cref_unival;

    [[nodiscard]] auto get_value(signed int n) const noexcept
        -> cref_unival;
    [[nodiscard]] auto get_value(signed long n) const noexcept
        -> cref_unival;
    [[nodiscard]] auto get_value(unsigned int n) const noexcept
        -> cref_unival;
    [[nodiscard]] auto get_value(unsigned long n) const noexcept
        -> cref_unival;
    [[nodiscard]] auto get_value(unsigned long long n) const noexcept
        -> cref_unival;

    [[nodiscard]] auto get_value(const char *key) const noexcept
        -> cref_unival;
    [[nodiscard]] auto get_value(const char8_t *key) const noexcept
        -> cref_unival;

    [[nodiscard]] auto by_key(signed int key) noexcept -> ref_unival;
    [[nodiscard]] auto by_key(signed long key) noexcept -> ref_unival;
    [[nodiscard]] auto by_key(unsigned int key) noexcept
        -> ref_unival;
    [[nodiscard]] auto by_key(unsigned long key) noexcept
        -> ref_unival;
    [[nodiscard]] auto by_key(unsigned long long key) noexcept
        -> ref_unival;

    [[nodiscard]] auto by_key(signed int key) const noexcept
        -> cref_unival;
    [[nodiscard]] auto by_key(signed long key) const noexcept
        -> cref_unival;
    [[nodiscard]] auto by_key(unsigned int key) const noexcept
        -> cref_unival;
    [[nodiscard]] auto by_key(unsigned long key) const noexcept
        -> cref_unival;
    [[nodiscard]] auto by_key(unsigned long long key) const noexcept
        -> cref_unival;

    [[nodiscard]] auto by_index(signed int key) noexcept
        -> ref_unival;
    [[nodiscard]] auto by_index(signed long key) noexcept
        -> ref_unival;
    [[nodiscard]] auto by_index(unsigned int key) noexcept
        -> ref_unival;
    [[nodiscard]] auto by_index(unsigned long key) noexcept
        -> ref_unival;
    [[nodiscard]] auto by_index(unsigned long long key) noexcept
        -> ref_unival;

    [[nodiscard]] auto by_index(signed int key) const noexcept
        -> cref_unival;
    [[nodiscard]] auto by_index(signed long key) const noexcept
        -> cref_unival;
    [[nodiscard]] auto by_index(unsigned int key) const noexcept
        -> cref_unival;
    [[nodiscard]] auto by_index(unsigned long key) const noexcept
        -> cref_unival;
    [[nodiscard]] auto by_index(unsigned long long key) const noexcept
        -> cref_unival;

    operator signed char() const noexcept;
    operator signed short() const noexcept;
    operator signed int() const noexcept;
    operator signed long() const noexcept;
    operator unsigned char() const noexcept;
    operator unsigned short() const noexcept;
    operator unsigned int() const noexcept;
    operator unsigned long() const noexcept;
    operator float() const noexcept;

    [[nodiscard]] auto operator[](signed int n) noexcept
        -> ref_unival;
    [[nodiscard]] auto operator[](signed int n) const noexcept
        -> cref_unival;

    [[nodiscard]] auto operator[](signed long n) noexcept
        -> ref_unival;
    [[nodiscard]] auto operator[](signed long n) const noexcept
        -> cref_unival;

    [[nodiscard]] auto operator[](unsigned int n) noexcept
        -> ref_unival;
    [[nodiscard]] auto operator[](unsigned int n) const noexcept
        -> cref_unival;

    [[nodiscard]] auto operator[](unsigned long n) noexcept
        -> ref_unival;
    [[nodiscard]] auto operator[](unsigned long n) const noexcept
        -> cref_unival;

    [[nodiscard]] auto operator[](unsigned long long n) noexcept
        -> ref_unival;
    [[nodiscard]] auto operator[](unsigned long long n) const noexcept
        -> cref_unival;

    [[nodiscard]] auto operator[](const char *key) noexcept
        -> ref_unival;
    [[nodiscard]] auto operator[](const char *key) const noexcept
        -> cref_unival;

    [[nodiscard]] auto operator[](const char8_t *key) noexcept
        -> ref_unival;
    [[nodiscard]] auto operator[](const char8_t *key) const noexcept
        -> cref_unival;

    [[nodiscard]] static auto logic_error() noexcept -> cref_unival;
    [[nodiscard]] static auto out_of_range() noexcept -> cref_unival;

  private:
    void assign(cref_vec value) noexcept;
    void assign(cref_composite value) noexcept;
    void assign(bool value) noexcept;
    void assign(signed long long value) noexcept;
    void assign(unsigned long long value) noexcept;
    void assign(double value) noexcept;
    void assign(std::string_view value) noexcept;
    void assign(std::wstring_view value) noexcept;
    void assign(std::u8string_view value) noexcept;
    void assign(std::u16string_view value) noexcept;
    void assign(std::u32string_view value) noexcept;
    void assign(span_cbyte value) noexcept;

    enum type_index : sl::index {
      n_uint = 1,
      n_bool,
      n_int,
      n_real,
      n_string,
      n_bytes,
      n_vector,
      n_composite
    };

    using field_type = std::variant<std::monostate,
                                    uint64_t,
                                    bool,
                                    int64_t,
                                    double,
                                    std::u8string,
                                    vbyte,
                                    vec,
                                    composite>;

    field_type m_data;
  };
}

#include "unival.impl.h"

#endif
