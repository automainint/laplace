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

#ifndef laplace_engine_prime_impact_h
#define laplace_engine_prime_impact_h

#include "../core/serial.h"
#include "basic_impact.h"
#include "protocol/ids.h"

namespace laplace::engine {
  /*  Basic class for any impact that can be serialized.
   */
  class prime_impact : public basic_impact {
  public:
    enum encoding_offset : sl::index {
      n_id    = 0,
      n_index = 2,
      n_time  = 10,
      n_actor = 18
    };

    using basic_impact::get_actor;
    using basic_impact::get_index;
    using basic_impact::get_time;
    using basic_impact::set_actor;
    using basic_impact::set_index;
    using basic_impact::set_time;

    /*  Max encoded impact size.
     */
    static constexpr sl::whole max_size =
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
    virtual void encode_to(span_byte bytes) const;

    [[nodiscard]] constexpr auto get_encoded_size() const -> sl::whole;

    static constexpr void set_actor(span_byte seq, sl::index id_actor);

    static constexpr void set_index(span_byte seq, sl::index n);

    static constexpr void set_time(span_byte seq, uint64_t time);

    [[nodiscard]] static constexpr auto is_unindexed(span_cbyte seq)
        -> bool;
    [[nodiscard]] static constexpr auto is_control(span_cbyte seq)
        -> bool;

    [[nodiscard]] static constexpr auto get_index_unsafe(span_cbyte seq)
        -> sl::index;
    [[nodiscard]] static constexpr auto get_time_unsafe(span_cbyte seq)
        -> uint64_t;
    [[nodiscard]] static constexpr auto get_actor_unsafe(span_cbyte seq)
        -> sl::index;

    [[nodiscard]] static constexpr auto get_id(span_cbyte seq)
        -> uint16_t;
    [[nodiscard]] static constexpr auto get_index(span_cbyte seq)
        -> sl::index;
    [[nodiscard]] static constexpr auto get_time(span_cbyte seq)
        -> uint64_t;
    [[nodiscard]] static constexpr auto get_actor(span_cbyte seq)
        -> sl::index;

    [[nodiscard]] static constexpr auto get_intval(span_cbyte seq,
                                                   sl::index  offset)
        -> intval;

    [[nodiscard]] static inline auto get_string(span_cbyte seq,
                                                sl::index  offset)
        -> std::u8string_view;

    [[nodiscard]] static inline auto get_string(span_cbyte seq,
                                                sl::index  offset,
                                                sl::whole  size)
        -> std::u8string_view;

  protected:
    constexpr void set_encoded_size(sl::whole size);

  private:
    [[nodiscard]] static constexpr auto is_unindexed_id(sl::index id)
        -> bool;
    [[nodiscard]] static constexpr auto is_control_id(sl::index id)
        -> bool;

    sl::whole m_encoded_size = 0;
  };

  /*  Sequentially consistent prime impact.
   */
  class sync_prime_impact : public prime_impact {
  public:
    constexpr sync_prime_impact();
    ~sync_prime_impact() override = default;
  };

  template <typename prime_impact_, typename... args_>
  [[nodiscard]] constexpr auto encode(args_... args) -> vbyte;

  using ptr_prime_impact = std::unique_ptr<prime_impact>;
}

#include "prime_impact.impl.h"

#endif
