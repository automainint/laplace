/*  laplace/graphics/program.h
 *
 *    GLSL program.
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

#ifndef laplace_graphics_program_h
#define laplace_graphics_program_h

#include "../core/defs.h"
#include <sstream>

namespace laplace::graphics {
  class program {
  public:
    using ref_program  = program &;
    using cref_program = const program &;

    program();
    ~program();

    program(program &&prog) noexcept;
    auto operator=(program &&prog) noexcept -> ref_program;

    void compute_shader(std::string_view code);
    void vertex_shader(std::string_view code);
    void geometry_shader(std::string_view code);
    void fragment_shader(std::string_view code);

    void compute_shader(ref_istream in);
    void vertex_shader(ref_istream in);
    void geometry_shader(ref_istream in);
    void fragment_shader(ref_istream in);

    void link();
    void use(bool flag) const;

    auto get_id() const -> uint32_t;

    program(cref_program) = delete;
    auto operator=(cref_program) -> ref_program = delete;

  private:
    void shader_source(uint32_t shader, ref_istream in);
    void log_program();
    void log_shader(uint32_t shader);

    uint32_t m_id;
  };
}

#endif
