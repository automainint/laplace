#include "mesh_buffer.h"
#include "../../platform/gldef.h"

using namespace laplace;
using namespace graphics;
using namespace tridi;
using namespace gl;
using namespace std;

mesh_buffer::mesh_buffer()
{
    glGenVertexArrays(1, &m_id_array);
    glGenBuffers(2, m_ids);

    m_count = 0;

    glBindVertexArray(m_id_array);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), reinterpret_cast<const void *>(offsetof(vertex, position)));
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), reinterpret_cast<const void *>(offsetof(vertex, normal)));
    glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(vertex), reinterpret_cast<const void *>(offsetof(vertex, texcoord)));
}

mesh_buffer::~mesh_buffer()
{
    glDeleteVertexArrays(1, &m_id_array);
    glDeleteBuffers(2, m_ids);
}

void mesh_buffer::setup(mesh_buffer::cref_vvertex vertices, cref_vuint32 indices, bool is_static)
{
    m_count = indices.size();

    glBindBuffer(GL_ARRAY_BUFFER, m_ids[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), vertices.data(), is_static ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ids[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

void mesh_buffer::update(cref_vvertex vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, m_ids[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
}

void mesh_buffer::render()
{
    glDrawElements(GL_TRIANGLES, static_cast<uint32_t>(m_count), GL_UNSIGNED_INT, nullptr);
}