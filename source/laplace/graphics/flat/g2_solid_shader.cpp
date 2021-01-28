#include "solid_shader.h"
#include "../../platform/gldef.h"

using namespace laplace;
using namespace graphics;
using namespace flat;
using namespace gl;
using namespace std;

solid_shader::solid_shader(ref_istream vert, ref_istream frag)
{
    m_program.vertex_shader(vert);
    m_program.fragment_shader(frag);

    m_program.link();

    m_program.use(true);

    n_position = glGetUniformLocation(m_program.get_id(), "mesh_position");
    n_scale = glGetUniformLocation(m_program.get_id(), "mesh_scale");
}

solid_shader::~solid_shader() { }

void solid_shader::use()
{
    m_program.use(true);
}

void solid_shader::set_position(cref_vec2 position)
{
    glUniform2fv(n_position, 1, position.v);
}

void solid_shader::set_scale(cref_vec2 scale)
{
    glUniform2fv(n_scale, 1, scale.v);
}
