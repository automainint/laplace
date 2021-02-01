#pragma once

#include "../impact.h"

namespace laplace::engine::protocol
{
    class non_inheritable { };
    class inheritable { };

    template <uint16_t id_, typename inheritance_tag_ = non_inheritable, typename base_class_ = impact>
    class elementary_event : public base_class_
    {
    public:
        static constexpr uint16_t   id      = id_;
        static constexpr size_t     size    = 10;

        ~elementary_event() override = default;

        constexpr elementary_event()
        {
            set_size(size);
        }

        constexpr elementary_event(size_t index)
        {
            set_order({ index });
            set_size(size);
        }

        inline void encode_to(std::span<uint8_t> bytes) const final
        {
            write_bytes(bytes, id, get_index64());
        }

        static constexpr auto scan(cref_vbyte seq)
        {
            return seq.size() == size && get_id(seq) == id;
        }

        static inline auto decode(cref_vbyte seq)
        {
            return elementary_event<id_, inheritance_tag_, base_class_> {
                get_index(seq)
            };
        }
    };

    template <uint16_t id_, typename base_class_>
    class elementary_event<id_, non_inheritable, base_class_> final : public base_class_
    {
    public:
        static constexpr uint16_t   id      = id_;
        static constexpr size_t     size    = 10;

        ~elementary_event() final = default;

        constexpr elementary_event()
        {
            set_size(size);
        }

        constexpr elementary_event(size_t index)
        {
            set_order({ index });
            set_size(size);
        }

        inline void encode_to(std::span<uint8_t> bytes) const final
        {
            write_bytes(bytes, id, get_index64());
        }

        static constexpr auto scan(cref_vbyte seq)
        {
            return seq.size() == size && get_id(seq) == id;
        }

        static inline auto decode(cref_vbyte seq)
        {
            return elementary_event<id_, non_inheritable, base_class_> {
                get_index(seq)
            };
        }
    };

    using client_enter  = elementary_event<ids::client_enter>;
    using client_leave  = elementary_event<ids::client_leave>;
    using client_ready  = elementary_event<ids::client_ready>;
    using server_action = elementary_event<ids::server_action>;
    using server_pause  = elementary_event<ids::server_pause>;
    using server_quit   = elementary_event<ids::server_quit>;

    using server_launch = elementary_event<ids::server_launch, inheritable>;
}
