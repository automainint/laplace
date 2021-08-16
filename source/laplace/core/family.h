/*  laplace/core/family.h
 *
 *      Universal data container. Indexed family data type.
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

#ifndef laplace_core_family_h
#define laplace_core_family_h

#include "defs.h"

namespace laplace::core {
  class family {
  public:
    using ref_family  = family &;
    using cref_family = const family &;

    using vfamily   = sl::vector<family>;
    using composite = sl::vector<std::pair<family, family>>;

    using cref_vfamily   = const vfamily &;
    using cref_composite = const composite &;

    family() noexcept  = default;
    ~family() noexcept = default;

    family(cref_family) noexcept = default;
    family(family &&) noexcept   = default;

    auto operator=(cref_family) noexcept -> ref_family = default;
    auto operator=(family &&) noexcept -> ref_family = default;

    family(cref_vfamily value) noexcept;
    family(cref_composite value) noexcept;
    family(bool value) noexcept;
    family(signed char value) noexcept;
    family(signed short value) noexcept;
    family(signed int value) noexcept;
    family(signed long long value) noexcept;
    family(unsigned char value) noexcept;
    family(unsigned short value) noexcept;
    family(unsigned int value) noexcept;
    family(unsigned long long value) noexcept;
    family(float value) noexcept;
    family(double value) noexcept;
    family(std::string_view value) noexcept;
    family(std::wstring_view value) noexcept;
    family(std::u8string_view value) noexcept;
    family(std::u16string_view value) noexcept;
    family(std::u32string_view value) noexcept;
    family(const char *value) noexcept;
    family(const wchar_t *value) noexcept;
    family(const char8_t *value) noexcept;
    family(const char16_t *value) noexcept;
    family(const char32_t *value) noexcept;
    family(span_cbyte value) noexcept;

    auto operator=(cref_vfamily value) noexcept -> ref_family;
    auto operator=(cref_composite value) noexcept -> ref_family;
    auto operator=(bool value) noexcept -> ref_family;
    auto operator=(signed char value) noexcept -> ref_family;
    auto operator=(signed short value) noexcept -> ref_family;
    auto operator=(signed int value) noexcept -> ref_family;
    auto operator=(signed long long value) noexcept -> ref_family;
    auto operator=(unsigned char value) noexcept -> ref_family;
    auto operator=(unsigned short value) noexcept -> ref_family;
    auto operator=(unsigned int value) noexcept -> ref_family;
    auto operator=(unsigned long long value) noexcept -> ref_family;
    auto operator=(float value) noexcept -> ref_family;
    auto operator=(double value) noexcept -> ref_family;
    auto operator=(std::string_view value) noexcept -> ref_family;
    auto operator=(std::wstring_view value) noexcept -> ref_family;
    auto operator=(std::u8string_view value) noexcept -> ref_family;
    auto operator=(std::u16string_view value) noexcept -> ref_family;
    auto operator=(std::u32string_view value) noexcept -> ref_family;
    auto operator=(const char *value) noexcept -> ref_family;
    auto operator=(const wchar_t *value) noexcept -> ref_family;
    auto operator=(const char8_t *value) noexcept -> ref_family;
    auto operator=(const char16_t *value) noexcept -> ref_family;
    auto operator=(const char32_t *value) noexcept -> ref_family;
    auto operator=(span_cbyte value) noexcept -> ref_family;

    [[nodiscard]] auto operator==(cref_family value) const noexcept
        -> bool;
    [[nodiscard]] auto operator!=(cref_family value) const noexcept
        -> bool;
    [[nodiscard]] auto operator<(cref_family value) const noexcept
        -> bool;
    [[nodiscard]] auto operator>(cref_family value) const noexcept
        -> bool;
    [[nodiscard]] auto operator<=(cref_family value) const noexcept
        -> bool;
    [[nodiscard]] auto operator>=(cref_family value) const noexcept
        -> bool;

    operator bool() const noexcept;
    operator signed long long() const noexcept;
    operator unsigned long long() const noexcept;
    operator double() const noexcept;

    auto operator[](signed long long index) noexcept -> ref_family;
    auto operator[](signed long long index) const noexcept
        -> cref_family;

    auto operator[](cref_family key) noexcept -> ref_family;
    auto operator[](cref_family key) const noexcept -> cref_family;

    /*  Returns true if empty.
     */
    auto is_empty() const noexcept -> bool;

    /*  Returns true if is boolean.
     */
    auto is_boolean() const noexcept -> bool;

    /*  Returns true if is integer.
     */
    auto is_integer() const noexcept -> bool;

    /*  Returns true if is unsigned integer.
     */
    auto is_uint() const noexcept -> bool;

    /*  Returns true if is floating-point number.
     */
    auto is_real() const noexcept -> bool;

    /*  Returns true if is string.
     */
    auto is_string() const noexcept -> bool;

    /*  Returns true if is byte array.
     */
    auto is_bytes() const noexcept -> bool;

    /*  Returns true if is vector.
     */
    auto is_vector() const noexcept -> bool;

    /*  Returns true if is composite.
     */
    auto is_composite() const noexcept -> bool;

    /*  Returns boolean value.
     */
    auto get_boolean() const noexcept -> bool;

    /*  Returns integer value.
     */
    auto get_integer() const noexcept -> signed long long;

    /*  Returns unsigned integer value.
     */
    auto get_uint() const noexcept -> unsigned long long;

    /*  Returns floating-point number value.
     */
    auto get_real() const noexcept -> double;

    /*  Returns string value.
     */
    auto get_string() const noexcept -> std::u8string_view;

    /*  Returns byte array.
     */
    auto get_bytes() const noexcept -> span_cbyte;

    /*  Return vector or composite size in elements.
     */
    auto get_size() const noexcept -> sl::whole;

    /*  Return true if composite has element by key.
     */
    auto has(cref_family key) const noexcept -> bool;

    /*  Set composite key by index.
     *
     *  Keys are sorted. Setting a key will change the keys order.
     */
    void set_key(signed long long index, cref_family k) noexcept;

    /*  Create a new key if needed.
     */
    void key(cref_family k) noexcept;

    /*  Get vector element by index.
     */
    auto value(signed long long index) noexcept -> ref_family;

    /*  Get composite element by key.
     */
    auto value(cref_family key) noexcept -> ref_family;

    /*  Get composite key by index.
     */
    auto get_key(signed long long index) const noexcept -> cref_family;

    /*  Get vector element by index.
     */
    auto get_value(signed long long index) const noexcept
        -> cref_family;

    /*  Get composite element by key.
     */
    auto get_value(cref_family key) const noexcept -> cref_family;

    /*  Get composite element by key.
     */
    auto by_key(signed long long key) noexcept -> ref_family;

    /*  Get composite element by key.
     */
    auto by_key(signed long long key) const noexcept -> cref_family;

    [[nodiscard]] auto compare(cref_family value) const noexcept -> int;

    /*  Merge composite data. Returns false
     *  if any field has been overwritten.
     */
    auto merge(cref_family f) noexcept -> bool;

    void set_key(signed int index, cref_family k) noexcept;
    auto value(signed int n) noexcept -> ref_family;
    auto value(unsigned int n) noexcept -> ref_family;
    auto value(unsigned long long n) noexcept -> ref_family;
    auto value(const char *key) noexcept -> ref_family;
    auto value(const char8_t *key) noexcept -> ref_family;
    auto get_key(signed int n) const noexcept -> cref_family;
    auto get_value(signed int n) const noexcept -> cref_family;
    auto get_value(unsigned int n) const noexcept -> cref_family;
    auto get_value(unsigned long long n) const noexcept -> cref_family;
    auto get_value(const char *key) const noexcept -> cref_family;
    auto get_value(const char8_t *key) const noexcept -> cref_family;
    auto by_key(signed int key) noexcept -> ref_family;
    auto by_key(signed int key) const noexcept -> cref_family;

    operator signed int() const noexcept;
    operator unsigned int() const noexcept;
    operator float() const noexcept;

    auto operator[](signed int n) noexcept -> ref_family;
    auto operator[](signed int n) const noexcept -> cref_family;

    auto operator[](unsigned int n) noexcept -> ref_family;
    auto operator[](unsigned int n) const noexcept -> cref_family;

    auto operator[](unsigned long long n) noexcept -> ref_family;
    auto operator[](unsigned long long n) const noexcept -> cref_family;

    auto operator[](const char *key) noexcept -> ref_family;
    auto operator[](const char *key) const noexcept -> cref_family;

    auto operator[](const char8_t *key) noexcept -> ref_family;
    auto operator[](const char8_t *key) const noexcept -> cref_family;

    static auto logic_error() noexcept -> cref_family;
    static auto out_of_range() noexcept -> cref_family;

  private:
    void assign(cref_vfamily value) noexcept;
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

    using field_type =
        std::variant<std::monostate, uint64_t, bool, int64_t, double,
                     std::u8string, vbyte, vfamily, composite>;

    field_type m_data;
  };

  using ref_family  = family::ref_family;
  using cref_family = family::cref_family;
  using vfamily     = family::vfamily;
  using composite   = family::composite;

  using ptr_family = std::shared_ptr<family>;
}

#include "family.impl.h"

#endif
