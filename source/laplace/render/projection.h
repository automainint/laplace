#pragma once

#include "defs.h"
#include <variant>

#undef near
#undef far

namespace laplace::render
{
    class projection
    {
    public:
        struct ortho
        {
            real left;
            real right;
            real bottom;
            real top;
            real near;
            real far;
        };

        struct perspective
        {
            real fovy;
            real aspect;
            real near;
            real far;
        };

        using cref_ortho = const ortho &;
        using cref_perspective = const perspective &;

        mat4 matrix;

        projection();
        ~projection();

        void set_matrix(cref_mat4 matrix);
        void set_ortho(real left, real right, real bottom, real top, real near, real far);
        void set_perspective(real fovy, real aspect, real near, real far);

        auto is_matrix() const -> bool;
        auto is_ortho() const -> bool;
        auto is_perspective() const -> bool;

        auto get_ortho() const -> cref_ortho;
        auto get_perspective() const -> cref_perspective;

    private:
        enum {
            n_matrix = 0,
            n_ortho,
            n_perspective
        };

        std::variant<
            std::monostate,
            ortho,
            perspective
        > m_data;
    };

    using cref_projection = const projection &;
}
