#include "utils.h"
#include "../platform/gldef.h"

using namespace laplace;
using namespace graphics;
using namespace gl;

void graphics::init()
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glFrontFace(GL_CCW);
}

void graphics::viewport(int x, int y, size_t width, size_t height)
{
    auto x0 = static_cast<int32_t>(x);
    auto y0 = static_cast<int32_t>(y);
    auto w = static_cast<uint32_t>(width);
    auto h = static_cast<uint32_t>(height);

    glViewport(x0, y0, w, h);
}

void graphics::clear(cref_vec4 color)
{
    glClearColor(color.red, color.green, color.blue, color.alpha);
    glClearDepth(1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void graphics::clear_color_buffer(cref_vec4 color)
{
    glClearColor(color.red, color.green, color.blue, color.alpha);
    glClear(GL_COLOR_BUFFER_BIT);
}

void graphics::prepare_ui()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
}

void graphics::prepare_render()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

void graphics::set_blend_enabled(bool is_enabled)
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
