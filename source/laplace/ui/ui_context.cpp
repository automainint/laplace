#include "context.h"

using namespace laplace;
using namespace ui;
using namespace elem;
using namespace graphics;
using namespace flat;
using namespace std;

weak_ptr<context> context::m_default;

context::context(text::ptr_renderer font)
{
    set_colors(default_colors);
    m_font = font;
}

void context::set_frame_size(size_t width, size_t height)
{
    m_frame_width = width;
    m_frame_height = height;
}

void context::set_colors(const graphics::vec4 colors[8])
{
    m_colors[0] = colors[0];
    m_colors[1] = colors[1];
    m_colors[2] = colors[2];
    m_colors[3] = colors[3];
    m_colors[4] = colors[4];
    m_colors[5] = colors[5];
    m_colors[6] = colors[6];
    m_colors[7] = colors[7];
}

void context::set_font(text::ptr_renderer font)
{
    m_font = font;
}

void context::setup(ptr_solid_shader shader)
{
    m_solid_shader = shader;
}

void context::setup(ptr_sprite_shader shader)
{
    m_sprite_shader = shader;
}

void context::render(cref_rect r, ref_texture tex)
{
    if (m_sprite_shader)
    {
        m_sprite_shader->use();
        m_sprite_shader->set_texture(0);
    }

    auto f = to_rectf(r);

    tex.bind_2d(0);

    m_sprite_buffer.render(f.x, f.y, f.width, f.height);
}

void context::render(panel::state panel_state)
{
    if (m_solid_shader)
    {
        m_solid_shader->use();
    }

    auto f = to_rectf(panel_state.rect);

    m_solid_buffer.render(f.x, f.y, f.width, f.height, m_colors[0]);
}

void context::render(button::state button_state)
{
    if (m_solid_shader)
    {
        m_solid_shader->use();
    }

    auto f = to_rectf(button_state.rect);

    size_t n0 = button_state.is_enabled ? 0 : 4;

    if (button_state.is_pressed)
    {
        m_solid_buffer.render(f.x, f.y, f.width, f.height, m_colors[n0 + 2]);
    }
    else if (button_state.has_cursor)
    {
        m_solid_buffer.render(f.x, f.y, f.width, f.height, m_colors[n0 + 1]);
    }
    else
    {
        m_solid_buffer.render(f.x, f.y, f.width, f.height, m_colors[n0 + 0]);
    }
}

void context::render(button::state button_state, u8string_view text)
{
    this->render(button_state);

    auto a = m_font->adjust(text);

    int dx = button_state.rect.width - a.width;
    int dy = button_state.rect.height - a.height;

    int x = button_state.rect.x + dx / 2;
    int y = button_state.rect.y + dy / 2;

    m_font->render(x, y, text);
}

void context::render(textedit::state textedit_state)
{
    if (m_solid_shader)
    {
        m_solid_shader->use();
    }

    auto f = to_rectf(textedit_state.rect);

    if (textedit_state.has_focus)
    {
        m_solid_buffer.render(f.x, f.y, f.width, f.height, m_colors[1]);
    }
    else
    {
        m_solid_buffer.render(f.x, f.y, f.width, f.height, m_colors[0]);
    }

    auto a = m_font->adjust(textedit_state.text);

    int dy = textedit_state.rect.height - a.height;

    int x = textedit_state.rect.x + dy / 2;
    int y = textedit_state.rect.y + dy / 2;

    m_font->render(x, y, textedit_state.text);
}

auto context::get_frame_width() const -> size_t
{
    return m_frame_width;
}

auto context::get_frame_height() const -> size_t
{
    return m_frame_height;
}

auto context::get_default() -> ptr_context
{
    auto p = m_default.lock();

    if (!p)
    {
        p = make_shared<context>();
        m_default = p;
    }

    return p;
}
