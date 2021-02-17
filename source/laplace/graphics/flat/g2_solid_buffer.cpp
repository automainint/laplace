#include "solid_buffer.h"
#include "../../platform/gldef.h"

using namespace laplace;
using namespace graphics;
using namespace flat;
using namespace gl;

solid_buffer::solid_buffer()
{
    glGenBuffers(1, &m_id);
}

solid_buffer::~solid_buffer()
{
    glDeleteBuffers(1, &m_id);
}

void solid_buffer::render(float x, float y, float width, float height, cref_vec4 color)
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

    v[0].color = color;
    v[1].color = color;
    v[2].color = color;
    v[3].color = color;

    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof v, v, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<const void *>(offsetof(vertex, position)));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<const void *>(offsetof(vertex, color)));

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
