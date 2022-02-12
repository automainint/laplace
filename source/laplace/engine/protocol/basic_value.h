/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_ENGINE_PROTOCOL_BASIC_VALUE_H
#define LAPLACE_ENGINE_PROTOCOL_BASIC_VALUE_H

#include "../prime_impact.h"

namespace laplace::engine::protocol {
  template <uint16_t id_, typename type_>
  class basic_value final : public prime_impact {
  public:
    enum encoding_offset : sl::index { n_value = 10 };

    static constexpr uint16_t  id   = id_;
    static constexpr sl::whole size = n_value + sizeof(type_);

    ~basic_value() noexcept final = default;

    constexpr basic_value() noexcept {
      this->set_encoded_size(size);
    }

    constexpr basic_value(type_ value) noexcept {
      this->set_encoded_size(size);
      this->m_value = value;
    }

    constexpr basic_value(sl::index n, type_ value) noexcept {
      this->set_index(n);
      this->set_encoded_size(size);
      this->m_value = value;
    }

    static constexpr auto get_value(span_cbyte seq) noexcept {
      return serial::rd<type_>(seq, n_value);
    }

    inline void encode_to(
        std::span<uint8_t> bytes) const noexcept final {
      serial::write_bytes(bytes, id, this->get_index64(),
                          this->m_value);
    }

    static constexpr auto scan(span_cbyte seq) noexcept -> bool {
      return seq.size() == size && prime_impact::get_id(seq) == id;
    }

    static inline auto decode(span_cbyte seq) noexcept {
      return basic_value { prime_impact::get_index(seq),
                           get_value(seq) };
    }

  private:
    type_ m_value = 0;
  };

  template <uint16_t id_, typename type_>
  class instant_value final : public prime_impact {
  public:
    enum encoding_offset : sl::index { n_value = 2 };

    static constexpr uint16_t  id   = id_;
    static constexpr sl::whole size = n_value + sizeof(type_);

    ~instant_value() noexcept final = default;

    constexpr instant_value() noexcept {
      this->set_encoded_size(size);
    }

    constexpr instant_value(type_ value) noexcept {
      this->set_encoded_size(size);
      this->m_value = value;
    }

    static constexpr auto get_value(span_cbyte seq) noexcept {
      return serial::rd<type_>(seq, n_value);
    }

    inline void encode_to(
        std::span<uint8_t> bytes) const noexcept final {
      serial::write_bytes(bytes, id, this->m_value);
    }

    static constexpr auto scan(span_cbyte seq) noexcept -> bool {
      return seq.size() == size && prime_impact::get_id(seq) == id;
    }

    static inline auto decode(span_cbyte seq) noexcept {
      return instant_value { get_value(seq) };
    }

  private:
    type_ m_value = 0;
  };

  using ping_request  = instant_value<ids::ping_request, sl::time64>;
  using ping_response = instant_value<ids::ping_response, sl::time64>;

  using server_reserve =
      basic_value<ids::server_reserve, sl::whole64>;

  using server_clock = basic_value<ids::server_clock, sl::time64>;
  using server_seed  = basic_value<ids::server_seed, sl::whole64>;
}

#endif
