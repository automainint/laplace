#include "shadow_shader.h"

using namespace laplace;
using namespace graphics;
using namespace tridi;
using namespace std;

shadow_shader::shadow_shader(ref_istream vert, ref_istream frag)
{
    m_program.vertex_shader(vert);
    m_program.fragment_shader(frag);

    m_program.link();

    m_program.use(true);
}

shadow_shader::~shadow_shader() { }

void shadow_shader::use()
{
    m_program.use(true);
}
