#include "context.h"
#include "../math/transform.h"
#include "../platform/opengl.h"
#include <cassert>

using namespace laplace;
using namespace render;
using namespace core;
using namespace graphics;
using namespace tridi;
using namespace std;

weak_ptr<context> context::m_default;

void context::setup(ptr_mesh_shader shader)
{
    m_mesh_shader = shader;
}

void context::setup(ptr_uvmap_shader shader)
{
    m_uvmap_shader = shader;
}

void context::setup(ptr_shadow_shader shader)
{
    m_shadow_shader = shader;
}

void context::setup(ptr_reflect_shader shader)
{
    m_reflect_shader = shader;
}

void context::setup(ptr_refract_shader shader)
{
    m_refract_shader = shader;
}

void context::set_scene(ptr_scene sce)
{
    m_scene = sce;
}

void context::set_settings(cref_settings sets)
{
    m_settings = sets;
}

void context::set_projection(cref_projection proj)
{
    m_projection = proj;
}

void context::render()
{

}

auto context::get_default() -> ptr_context
{
    ptr_context result = m_default.lock();

    if (!result)
    {
        result = make_shared<context>();
        m_default = result;
    }

    return result;
}
