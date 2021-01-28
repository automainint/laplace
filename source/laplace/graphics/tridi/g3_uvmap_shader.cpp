#include "uvmap_shader.h"
#include "../../platform/gldef.h"

using namespace laplace;
using namespace graphics;
using namespace tridi;
using namespace gl;
using namespace std;

uvmap_shader::uvmap_shader(ref_istream vert, ref_istream frag)
{
    m_program.vertex_shader(vert);
    m_program.fragment_shader(frag);

    m_program.link();

    m_program.use(true);

    n_center_position = glGetUniformLocation(m_program.get_id(), "center_position");
    n_object_rotation = glGetUniformLocation(m_program.get_id(), "object_rotation");
    n_object_transform = glGetUniformLocation(m_program.get_id(), "object_transform");
    n_light_position = glGetUniformLocation(m_program.get_id(), "light_position");
    n_light_color = glGetUniformLocation(m_program.get_id(), "light_color");
    n_light_ambient = glGetUniformLocation(m_program.get_id(), "light_ambient");
    n_light_diffuse = glGetUniformLocation(m_program.get_id(), "light_diffuse");
    n_light_specular = glGetUniformLocation(m_program.get_id(), "light_specular");
    n_light_focus = glGetUniformLocation(m_program.get_id(), "light_focus");
    n_material_colormap = glGetUniformLocation(m_program.get_id(), "material_colormap");
    n_material_ambient = glGetUniformLocation(m_program.get_id(), "material_ambient");
    n_material_diffsue = glGetUniformLocation(m_program.get_id(), "material_diffsue");
    n_material_specular = glGetUniformLocation(m_program.get_id(), "material_specular");
    n_light_shadowmap = glGetUniformLocation(m_program.get_id(), "light_shadowmap");
}

uvmap_shader::~uvmap_shader() { }

void uvmap_shader::use()
{
    m_program.use(true);
}

void uvmap_shader::set_object(uvmap_shader::cref_object obj)
{
    glUniformMatrix3fv(n_object_rotation, 1, false, obj.rotation.v);
    glUniformMatrix4fv(n_object_transform, 1, false, obj.transform.v);
}

void uvmap_shader::set_view(uvmap_shader::cref_view view)
{
    glUniform3fv(n_center_position, 1, view.center_position.v);
}

void uvmap_shader::set_material(uvmap_shader::cref_material mat)
{
    glUniform1i(n_material_colormap, static_cast<int32_t>(mat.colormap_id));
    glUniform1f(n_material_diffsue, mat.diffuse);
    glUniform1f(n_material_specular, mat.specular);
}

void uvmap_shader::set_light(uvmap_shader::cref_light light)
{
    glUniform1i(n_light_shadowmap, static_cast<int32_t>(light.shadowmap_id));
    glUniform3fv(n_light_position, 1, light.position.v);
    glUniform3fv(n_light_color, 1, light.color.v);
    glUniform1f(n_light_ambient, light.ambient);
    glUniform1f(n_light_diffuse, light.diffuse);
    glUniform1f(n_light_specular, light.specular);
    glUniform1f(n_light_focus, light.focus);
}
