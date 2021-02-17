#include "sprite_buffer.h"
#include "../../platform/gldef.h"

using namespace laplace;
using namespace graphics;
using namespace flat;
using namespace gl;

sprite_buffer::sprite_buffer()
{
    glGenBuffers(1, &m_id);
}

sprite_buffer::~sprite_buffer()
{
    glDeleteBuffers(1, &m_id);
}

void sprite_buffer::render(float x, float y, float width, float height)
{
    vertex v[4];

    v[0].position.x() = x;
    v[0].position.y() = y;
    v[1].position.x() = x;
    v[1].position.y() = y + height;
    v[2].position.x() = x + width;
    v[2].position.y() = y;
    v[3].position.x() = x + width;
    v[3].position.y() = y + height;

    v[0].texcoord = { 0.f, 1.f };
    v[1].texcoord = { 0.f, 0.f };
    v[2].texcoord = { 1.f, 1.f };
    v[3].texcoord = { 1.f, 0.f };

    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof v, v, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<const void *>(offsetof(vertex, position)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<const void *>(offsetof(vertex, texcoord)));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
