/*  laplace/engine/basic_factory.h
 *
 *      Basic Impact factory.
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

#ifndef __laplace__engine_basic_factory__
#define __laplace__engine_basic_factory__

#include "basic_impact.h"

namespace laplace::engine {
  class basic_factory {
  public:
    basic_factory()          = default;
    virtual ~basic_factory() = default;

    virtual auto parse(std::string_view command) const -> vbyte;
    virtual auto print(cref_vbyte seq) const -> std::string;

    virtual auto decode(cref_vbyte seq) const -> ptr_impact;

    auto parse_multi(std::string_view commands) const
        -> std::vector<vbyte>;
    auto print_multi(std::span<const cref_vbyte> seqs) const
        -> std::string;

    static auto parse_native(std::span<const std::string_view> table,
                             std::string_view command) -> vbyte;

    static auto print_native(std::span<const std::string_view> table,
                             cref_vbyte seq) -> std::string;

    static auto decode_native(cref_vbyte seq) -> ptr_impact;

    template <typename impact_type>
    static auto make(cref_vbyte seq) -> ptr_impact;
  };

  using ptr_factory = std::shared_ptr<basic_factory>;
}

#include "basic_factory.impl.h"

#endif
