#pragma once

#include "impact.h"

namespace laplace::engine
{
    /*  Impact factory.
     */
    class factory
    {
    public:
        factory();
        virtual ~factory();

        virtual auto parse(std::string_view command) -> vbyte;
        virtual auto print(cref_vbyte seq) -> std::string;

        virtual auto decode(cref_vbyte seq) -> ptr_impact;

        static auto parse_native(cref_vstring table, std::string_view command) -> vbyte;
        static auto print_native(cref_vstring table, cref_vbyte seq) -> std::string;
        static auto decode_native(cref_vbyte seq) -> ptr_impact;

        template <typename impact_type>
        static auto make(cref_vbyte seq) -> ptr_impact;
    };

    using ptr_factory = std::shared_ptr<factory>;
}

#include "factory.impl.h"
