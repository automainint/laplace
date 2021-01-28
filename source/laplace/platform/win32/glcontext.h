/*  laplace/platform/win32/input.h
 *
 *      Win32 OpenGL context declarations.
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#pragma once

#include "wgl.h"
#include "windef.h"
#include "../events.h"
#include <memory>

namespace laplace::win32
{
    class window;

    class glcontext
    {
    public:
        static bool is_forward_compatible;

        glcontext(std::shared_ptr<window> win);
        ~glcontext();

        void swap_buffers();

    private:
        void cleanup();

        std::shared_ptr<window> m_window;

        HDC     m_hDC   = nullptr;
        HGLRC   m_hRC   = nullptr;
    };
}
