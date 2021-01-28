/*  laplace/engine/eventorder.h
 *
 *      Trivial class that contains event order information.
 *      Allows to sort tree-structured events.
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

#include "../core/defs.h"

namespace laplace::engine
{
    class eventorder
    {
    public:
        /*  Maximum event tree depth.
         */
        static constexpr uint8_t max_depth = 8;

        constexpr eventorder() = default;
        constexpr eventorder(size_t index);
        ~eventorder() = default;

        constexpr eventorder(const eventorder &) = default;
        constexpr eventorder(eventorder &&) = default;
        constexpr eventorder &operator =(const eventorder &) = default;
        constexpr eventorder &operator =(eventorder &&) = default;

        /*  Spawn a child event.
         */
        constexpr auto spawn(ref_uint child_count) const -> eventorder;

        constexpr auto operator <(const eventorder &order) const -> bool;

        constexpr auto get_index() const -> size_t;

    private:
        constexpr eventorder(const eventorder &parent, size_t index);

        std::array<size_t, max_depth>   m_indices   = { 0 };
        uint8_t                         m_size      = 0;
    };

    using cref_eventorder = const eventorder &;
}

#include "eventorder.impl.h"
