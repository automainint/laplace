#include "reflect_shader.h"
#include "../../platform/gldef.h"

using namespace laplace;
using namespace graphics;
using namespace tridi;
using namespace gl;
using namespace std;

reflect_shader::reflect_shader(ref_istream vert, ref_istream frag)
{
    m_program.vertex_shader(vert);
    m_program.fragment_shader(frag);

    m_program.link();

    m_program.use(true);

    n_full_transform = glGetUniformLocation(m_program.get_id(), "full_transform");
    n_material_colormap = glGetUniformLocation(m_program.get_id(), "material_colormap");
}

reflect_shader::~reflect_shader()
{
}

void reflect_shader::use()
{
    m_program.use(true);
}

void reflect_shader::set_view(reflect_shader::cref_view view)
{
    glUniformMatrix4fv(n_full_transform, 1, false, view.full_transform.v);
}

void reflect_shader::set_material(reflect_shader::cref_material mat)
{
    glUniform1i(n_material_colormap, static_cast<int32_t>(mat.colormap_id));
}
