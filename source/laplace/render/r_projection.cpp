#include "projection.h"
#include "../math/transform.h"

using namespace laplace;
using namespace render;
using namespace std;

projection::projection()
{
    matrix = mat4(real(1));
}

projection::~projection() { }

void projection::set_matrix(cref_mat4 matrix)
{
    this->m_data = monostate();
    this->matrix = matrix;
}

void projection::set_ortho(real left, real right, real bottom, real top, real near, real far)
{
    m_data = ortho {
        left,
        right,
        bottom,
        top,
        near,
        far
    };

    matrix = math::ortho_matrix(left, right, bottom, top, near, far);
}

void projection::set_perspective(real fovy, real aspect, real near, real far)
{
    m_data = perspective {
        fovy,
        aspect,
        near,
        far
    };

    matrix = math::perspective_matrix(fovy, aspect, near, far);
}

auto projection::is_matrix() const -> bool
{
    return m_data.index() == n_matrix;
}

auto projection::is_ortho() const -> bool
{
    return m_data.index() == n_ortho;
}

auto projection::is_perspective() const -> bool
{
    return m_data.index() == n_perspective;
}

auto projection::get_ortho() const -> projection::cref_ortho
{
    assert(m_data.index() == n_ortho);

    if (m_data.index() != n_ortho)
    {
        static ortho nil { 0, 0, 0, 0, 0, 0 };
        return nil;
    }

    return get<n_ortho>(m_data);
}

auto projection::get_perspective() const -> projection::cref_perspective
{
    assert(m_data.index() == n_perspective);

    if (m_data.index() != n_perspective)
    {
        static perspective nil { 0, 0, 0, 0 };
        return nil;
    }

    return get<n_perspective>(m_data);
}
