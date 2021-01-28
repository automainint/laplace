#include "mesh_shader.h"
#include "../../platform/gldef.h"

using namespace laplace;
using namespace graphics;
using namespace tridi;
using namespace gl;
using namespace std;

mesh_shader::mesh_shader(ref_istream vert, ref_istream frag)
{
    m_program.vertex_shader(vert);
    m_program.fragment_shader(frag);

    m_program.link();

    m_program.use(true);

    n_object_rotation = glGetUniformLocation(m_program.get_id(), "object_rotation");
    n_object_transform = glGetUniformLocation(m_program.get_id(), "object_transform");
    n_view_transform = glGetUniformLocation(m_program.get_id(), "view_transform");
    n_eye_position = glGetUniformLocation(m_program.get_id(), "eye_position");
    n_light_position = glGetUniformLocation(m_program.get_id(), "light_position");
    n_material_colormap = glGetUniformLocation(m_program.get_id(), "material_colormap");
    n_material_diffsue = glGetUniformLocation(m_program.get_id(), "material_diffsue");
    n_material_specular = glGetUniformLocation(m_program.get_id(), "material_specular");
    n_light_color = glGetUniformLocation(m_program.get_id(), "light_color");
    n_light_ambient = glGetUniformLocation(m_program.get_id(), "light_ambient");
    n_light_diffuse = glGetUniformLocation(m_program.get_id(), "light_diffuse");
    n_light_specular = glGetUniformLocation(m_program.get_id(), "light_specular");
    n_light_focus = glGetUniformLocation(m_program.get_id(), "light_focus");
    n_light_shadowmap = glGetUniformLocation(m_program.get_id(), "light_shadowmap");
}

mesh_shader::~mesh_shader() { }

void mesh_shader::use()
{
    m_program.use(true);
}

void mesh_shader::set_object(mesh_shader::cref_object obj)
{
    glUniformMatrix3fv(n_object_rotation, 1, false, obj.rotation.v);
    glUniformMatrix4fv(n_object_transform, 1, false, obj.transform.v);
}

void mesh_shader::set_view(mesh_shader::cref_view view)
{
    glUniformMatrix4fv(n_view_transform, 1, false, view.transform.v);
    glUniform3fv(n_eye_position, 1, view.eye_position.v);
}

void mesh_shader::set_material(mesh_shader::cref_material mat)
{
    glUniform1i(n_material_colormap, static_cast<int32_t>(mat.colormap_id));
    glUniform1f(n_material_diffsue, mat.diffuse);
    glUniform1f(n_material_specular, mat.specular);
}

void mesh_shader::set_light(mesh_shader::cref_light light)
{
    glUniform1i(n_light_shadowmap, static_cast<int32_t>(light.shadowmap_id));
    glUniform3fv(n_light_position, 1, light.position.v);
    glUniform3fv(n_light_color, 1, light.color.v);
    glUniform1f(n_light_ambient, light.ambient);
    glUniform1f(n_light_diffuse, light.diffuse);
    glUniform1f(n_light_specular, light.specular);
    glUniform1f(n_light_focus, light.focus);
}

void mesh_shader::prepare()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void mesh_shader::blending(bool is_enabled)
{
    if (is_enabled)
    {
        glEnable(GL_BLEND);
    }
    else
    {
        glDisable(GL_BLEND);
    }
}
