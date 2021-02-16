/*  laplace/engine/prime_impact.h
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

#ifndef __laplace__engine_prime_impact__
#define __laplace__engine_prime_impact__

#include "basic_impact.h"

namespace laplace::engine {
  /*  Basic class for any impact that can be serialized.
   */
  class prime_impact : public basic_impact {
  public:
    using basic_impact::get_actor;
    using basic_impact::get_index;
    using basic_impact::get_time;

    /*  Max encoded impact size.
     */
    static constexpr size_t max_size =
        std::numeric_limits<uint16_t>::max();

    ~prime_impact() override = default;

    /*  Encode into byte sequence. The sequence can be decoded
     *  by the Impact Factory.
     */
    [[nodiscard]] auto encode() const -> vbyte;

    /*  Serialization.
     *
     *  TODO
     *  Make it constexpr.
     */
    virtual void encode_to(std::span<uint8_t> bytes) const;

    [[nodiscard]] constexpr auto get_encoded_size() const
        -> size_t;

    static constexpr void set_index(ref_vbyte seq, size_t index);

    [[nodiscard]] static constexpr auto get_id_unsafe(cref_vbyte seq)
        -> uint16_t;
    [[nodiscard]] static constexpr auto get_index_unsafe(
        cref_vbyte seq) -> size_t;
    [[nodiscard]] static constexpr auto get_time_unsafe(
        cref_vbyte seq) -> uint64_t;
    [[nodiscard]] static constexpr auto get_actor_unsafe(
        cref_vbyte seq) -> size_t;

    [[nodiscard]] static constexpr auto get_id(cref_vbyte seq)
        -> uint16_t;
    [[nodiscard]] static constexpr auto get_index(cref_vbyte seq)
        -> size_t;
    [[nodiscard]] static constexpr auto get_time(cref_vbyte seq)
        -> uint64_t;
    [[nodiscard]] static constexpr auto get_actor(cref_vbyte seq)
        -> size_t;

    [[nodiscard]] static inline auto get_string(cref_vbyte seq,
                                                size_t offset)
        -> std::u8string_view;

    [[nodiscard]] static inline auto get_string(cref_vbyte seq,
                                                size_t offset,
                                                size_t size)
        -> std::u8string_view;

  protected:
    constexpr void set_encoded_size(size_t size);

  private:
    size_t m_encoded_size = 0;
  };

  /*  Sequentially consistent prime impact.
   */
  class sync_prime_impact : public prime_impact {
  public:
    constexpr sync_prime_impact();
    ~sync_prime_impact() override = default;
  };

  template <typename prime_impact_>
  static constexpr auto encode(const prime_impact_ &ev) -> vbyte;

  using ptr_prime_impact = std::shared_ptr<prime_impact>;
}

#include "prime_impact.impl.h"

#endif
