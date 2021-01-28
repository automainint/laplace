#include "texture.h"
#include "../platform/gldef.h"

using namespace laplace;
using namespace core;
using namespace graphics;
using namespace gl;
using namespace std;

static constexpr GLenum gl_wrap[3] = {
    GL_CLAMP,
    GL_REPEAT,
    GL_MIRRORED_REPEAT
};

texture::texture()
{
    static element::type type_texture;
    set_type_id(type_texture.get_id());

    m_ok = true;
    m_wrap[0] = m_wrap[1] = m_wrap[2] = texture::clamp;
    glGenTextures(1, &m_id);
}

texture::~texture()
{
    if (m_ok)
    {
        glDeleteTextures(1, &m_id);
    }
}

texture::texture(texture &&tex) noexcept
{
    m_ok = tex.m_ok;
    m_id = tex.m_id;
    memcpy(m_wrap, tex.m_wrap, sizeof m_wrap);

    tex.m_ok = false;
}

auto texture::operator =(texture &&tex) noexcept -> texture &
{
    if (m_ok)
    {
        glDeleteTextures(1, &m_id);
    }

    m_ok = tex.m_ok;
    m_id = tex.m_id;
    memcpy(m_wrap, tex.m_wrap, sizeof m_wrap);

    tex.m_ok = false;

    return *this;
}

void texture::bind_2d(size_t index)
{
    glActiveTexture(static_cast<uint32_t>(GL_TEXTURE0 + index));
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void texture::bind_3d(size_t index)
{
    glActiveTexture(static_cast<uint32_t>(GL_TEXTURE0 + index));
    glBindTexture(GL_TEXTURE_3D, m_id);
}

void texture::set_wrap_x(wrap mode)
{
    m_wrap[0] = gl_wrap[mode];
}

void texture::set_wrap_y(wrap mode)
{
    m_wrap[1] = gl_wrap[mode];
}

void texture::set_wrap_z(wrap mode)
{
    m_wrap[2] = gl_wrap[mode];
}

void texture::image_2d_linear(cref_image img)
{
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    auto width = static_cast<uint32_t>(img.get_width());
    auto height = static_cast<uint32_t>(img.get_height());

    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.get_data());
}

void texture::image_2d_nearest(cref_image img)
{
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    auto width = static_cast<uint32_t>(img.get_width());
    auto height = static_cast<uint32_t>(img.get_height());

    glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.get_data());
}

void texture::mipmaps_2d_linear(cref_image img)
{
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    for (size_t level = 0; level < img.get_mip_count(); level++)
    {
        auto mip = img.mip(level);
        auto n = static_cast<int32_t>(level);
        auto width = static_cast<uint32_t>(mip->get_width());
        auto height = static_cast<uint32_t>(mip->get_height());

        glTexImage2D(GL_TEXTURE_2D, n, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mip->get_data());
    }
}

void texture::mipmaps_2d_nearest(cref_image img)
{
    glBindTexture(GL_TEXTURE_2D, m_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_wrap[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_wrap[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    for (size_t level = 0; level < img.get_mip_count(); level++)
    {
        auto mip = img.mip(level);
        auto n = static_cast<int32_t>(level);
        auto width = static_cast<uint32_t>(mip->get_width());
        auto height = static_cast<uint32_t>(mip->get_height());

        glTexImage2D(GL_TEXTURE_2D, n, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, mip->get_data());
    }
}

void texture::image_3d_linear(cref_image img)
{
    glBindTexture(GL_TEXTURE_3D, m_id);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, m_wrap[0]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, m_wrap[1]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, m_wrap[2]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    auto w = static_cast<uint32_t>(img.get_width());
    auto h = static_cast<uint32_t>(img.get_height());
    auto d = static_cast<uint32_t>(img.get_depth());

    glTexImage3D(GL_TEXTURE_3D, 0, 4, w, h, d, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.get_data());
}

void texture::image_3d_nearest(cref_image img)
{
    glBindTexture(GL_TEXTURE_3D, m_id);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, m_wrap[0]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, m_wrap[1]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, m_wrap[2]);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    auto w = static_cast<uint32_t>(img.get_width());
    auto h = static_cast<uint32_t>(img.get_height());
    auto d = static_cast<uint32_t>(img.get_depth());

    glTexImage3D(GL_TEXTURE_3D, 0, 4, w, h, d, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.get_data());
}

auto texture::get_id() const -> uint32_t
{
    return m_id;
}
