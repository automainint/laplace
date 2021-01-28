/*  laplace/engine/impact.impl.h
 *
 *      Impact constexpr implementation.
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

#pragma once

namespace laplace::engine
{
    constexpr void impact::set_order(cref_eventorder order)
    {
        this->m_order = order;
    }

    constexpr void impact::set_time(uint64_t time)
    {
        this->m_time = time;
    }

    constexpr void impact::set_actor(size_t id)
    {
        this->m_id_actor = id;
    }

    inline auto impact::encode() const -> laplace::vbyte
    {
        vbyte seq(get_encoded_size());
        encode_to(seq);
        return seq;
    }

    inline void impact::encode_to(std::span<uint8_t> bytes) const { }

    constexpr auto impact::get_encoded_size() const -> size_t
    {
        return this->m_size;
    }

    constexpr auto impact::get_index() const -> size_t
    {
        return this->m_order.get_index();
    }

    constexpr auto impact::get_order() const -> cref_eventorder
    {
        return this->m_order;
    }

    constexpr auto impact::get_time() const -> uint64_t
    {
        return this->m_time;
    }

    constexpr auto impact::get_actor() const -> size_t
    {
        return this->m_id_actor;
    }

    constexpr auto impact::is_async() const -> bool
    {
        return this->m_is_async;
    }

    constexpr auto impact::get_index64() const -> uint64_t
    {
        return static_cast<uint64_t>(this->get_index());
    }

    constexpr auto impact::get_time64() const -> uint64_t
    {
        return static_cast<uint64_t>(this->get_time());
    }

    constexpr auto impact::get_actor64() const -> uint64_t
    {
        return static_cast<uint64_t>(this->get_actor());
    }

    inline void impact::set_index(ref_vbyte seq, size_t index)
    {
        using namespace protocol;

        if (impact::get_id(seq) >= ids::_unindexed_count)
            wr<uint64_t>(seq, impact::n_index, index);
    }

    inline auto impact::get_id_unsafe(cref_vbyte seq) -> uint16_t
    {
        return rd<uint16_t>(seq, impact::n_id);
    }

    inline auto impact::get_index_unsafe(cref_vbyte seq) -> size_t
    {
        return as_index(rd<uint64_t>(seq, impact::n_index));
    }

    inline auto impact::get_time_unsafe(cref_vbyte seq) -> uint64_t
    {
        return rd<uint64_t>(seq, impact::n_time);
    }

    inline auto impact::get_id(cref_vbyte seq) -> uint16_t
    {
        using namespace protocol;

        return
            seq.size() >= sizeof(uint16_t) ?
            rd<uint16_t>(seq, impact::n_id) : ids::undefined;
    }

    inline auto impact::get_index(cref_vbyte seq) -> size_t
    {
        using namespace protocol;

        return
            impact::get_id(seq) < ids::_unindexed_count ?
            id_undefined : as_index(rd<uint64_t>(seq, impact::n_index));
    }

    inline auto impact::get_time(cref_vbyte seq) -> uint64_t
    {
        using namespace protocol;

        return
            impact::get_id(seq) < ids::_control_count ?
            time_undefined : rd<uint64_t>(seq, impact::n_time);
    }

    constexpr void impact::set_async(bool is_async)
    {
        this->m_is_async = is_async;
    }

    constexpr void impact::set_size(size_t size)
    {
        this->m_size = size;
    }

    constexpr auto impact::order_of_child(ref_uint count) const -> eventorder
    {
        return this->m_order.spawn(count);
    }

    constexpr sync_impact::sync_impact()
    {
        this->set_async(false);
    }

    template <typename impact_type>
    inline auto gen() -> impact_gen
    {
        using namespace std;

        return [] -> ptr_impact {
            return make_shared<impact_type>();
        };
    }
}
