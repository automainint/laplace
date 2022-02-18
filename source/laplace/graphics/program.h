/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_GRAPHICS_PROGRAM_H
#define LAPLACE_GRAPHICS_PROGRAM_H

#include "../core/defs.h"
#include "../core/log.h"
#include <sstream>

namespace laplace::graphics {
  class program {
  public:
    using ref_program  = program &;
    using cref_program = const program &;

    log_handler log = get_global_log();
    
    program();
    ~program();

    program(program &&prog) noexcept;
    auto operator=(program &&prog) noexcept -> ref_program;

    void compute_shader(std::string_view code);
    void vertex_shader(std::string_view code);
    void geometry_shader(std::string_view code);
    void fragment_shader(std::string_view code);

    void compute_shader(std::istream &in);
    void vertex_shader(std::istream &in);
    void geometry_shader(std::istream &in);
    void fragment_shader(std::istream &in);

    void link();
    void use(bool flag) const;

    auto get_id() const -> uint32_t;

    program(cref_program) = delete;
    auto operator=(cref_program) -> ref_program = delete;

  private:
    void shader_source(uint32_t shader, std::istream &in);
    void log_program();
    void log_shader(uint32_t shader);

    uint32_t m_id;
  };
}

#endif
