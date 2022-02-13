/*  laplace/graphics/g_program.cpp
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

#include "program.h"

#include "../platform/gldef.h"

namespace laplace::graphics {
  using std::string_view, std::string, std::istringstream,
      std::vector;

  using namespace gl;

  program::program() {
    m_id = glCreateProgram();
  }

  program::~program() {
    if (m_id != 0) {
      glDeleteProgram(m_id);
    }
  }

  program::program(program &&prog) noexcept {
    m_id      = prog.m_id;
    prog.m_id = 0;
  }

  auto program::operator=(program &&prog) noexcept -> ref_program {
    if (m_id != prog.m_id) {
      if (m_id != 0) {
        glDeleteProgram(m_id);
      }

      m_id      = prog.m_id;
      prog.m_id = 0;
    }

    return *this;
  }

  void program::compute_shader(string_view code) {
    auto s  = string(code);
    auto in = istringstream(s);
    compute_shader(in);
  }

  void program::vertex_shader(string_view code) {
    auto s  = string(code);
    auto in = istringstream(s);
    vertex_shader(in);
  }

  void program::geometry_shader(string_view code) {
    auto s  = string(code);
    auto in = istringstream(s);
    geometry_shader(in);
  }

  void program::fragment_shader(string_view code) {
    auto s  = string(code);
    auto in = istringstream(s);
    fragment_shader(in);
  }

  void program::compute_shader(std::istream &in) {
    auto shader = glCreateShader(GL_COMPUTE_SHADER);
    shader_source(shader, in);
  }

  void program::vertex_shader(std::istream &in) {
    auto shader = glCreateShader(GL_VERTEX_SHADER);
    shader_source(shader, in);
  }

  void program::geometry_shader(std::istream &in) {
    auto shader = glCreateShader(GL_GEOMETRY_SHADER);
    shader_source(shader, in);
  }

  void program::fragment_shader(std::istream &in) {
    auto shader = glCreateShader(GL_FRAGMENT_SHADER);
    shader_source(shader, in);
  }

  void program::link() {
    glLinkProgram(m_id);

    int32_t status;
    glGetProgramiv(m_id, GL_LINK_STATUS, &status);

    if (!status) {
      log_program();
    }
  }

  void program::use(bool flag) const {
    glUseProgram(flag ? m_id : 0);
  }

  auto program::get_id() const -> uint32_t {
    return m_id;
  }

  void program::shader_source(uint32_t shader, std::istream &in) {
    auto source = vector<string> {};

    if (in) {
      while (!in.eof()) {
        auto line = string {};
        getline(in, line);
        line.append("\n");
        source.emplace_back(line);
      }
    }

    auto p = vector<const char *> {};

    for (size_t i = 0; i < source.size(); i++) {
      p.emplace_back(source[i].c_str());
    }

    glShaderSource(shader, static_cast<GLsizei>(p.size()), p.data(),
                   nullptr);

    glCompileShader(shader);

    int32_t status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status) {
      glAttachShader(m_id, shader);
    } else {
      log_shader(shader);
      glDeleteShader(shader);
    }
  }

  void program::log_program() {
    int32_t length;
    glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);
    string info_log(static_cast<size_t>(length), '\0');
    glGetProgramInfoLog(m_id, length, &length, &info_log[0]);
    info_log.resize(static_cast<size_t>(length));
    verb(info_log, "Graphics/Program");
  }

  void program::log_shader(uint32_t shader) {
    int32_t length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    string info_log(static_cast<size_t>(length), '\0');
    glGetShaderInfoLog(shader, length, &length, &info_log[0]);
    info_log.resize(static_cast<size_t>(length));
    verb(info_log, "Graphics/Program");
  }
}
