/*  laplace/platform/win32/win32_glcontext.cpp
 *
 *      Win32 OpenGL context implementation.
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

#define WIN32_LEAN_AND_MEAN
#define LAPLACE_WINDOWS_INCLUDED
#include <windows.h>

#include "glcontext.h"
#include "window.h"
#include "../gldef.h"
#include <iostream>
#include <algorithm>

extern "C" {

    /*  Require high performance for Nvidia.
     */
    __declspec(dllexport) uint32_t NvOptimusEnablement                  = 1;

    /*  Require high performance for AMD.
     */
    __declspec(dllexport) uint32_t AmdPowerXpressRequestHighPerformance = 1;
}

using namespace laplace;
using namespace win32;
using namespace std;

bool glcontext::is_forward_compatible = false;

glcontext::glcontext(shared_ptr<window> win)
{
    if (!win)
    {
        cerr << __FUNCTION__ << ": No window." << endl;
        return;
    }

    if (!gl::platform_init())
    {
        cerr << __FUNCTION__ << ": WGL initialization failed." << endl;
        return;
    }

    m_window = win;

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd, 0, sizeof pfd);

    pfd.nSize = sizeof pfd;
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    if (is_forward_compatible)
    {
        /*  OpenGL forward-compatibility.
         */
        pfd.iLayerType = PFD_MAIN_PLANE;
    }

    m_hDC = GetDC(win->get_native_handle());

    if (!m_hDC)
    {
        cerr << __FUNCTION__ << ": GetDC failed." << endl;
        cleanup();
        return;
    }

    int pf = ChoosePixelFormat(m_hDC, &pfd);

    if (!SetPixelFormat(m_hDC, pf, &pfd))
    {
        cerr << __FUNCTION__ << ": SetPixelFormat failed." << endl;
        cleanup();
        return;
    }

    m_hRC = gl::wglCreateContext(m_hDC);

    if (!m_hRC)
    {
        cerr << __FUNCTION__ << ": wglCreateContext failed." << endl;
        cleanup();
        return;
    }

    if (!gl::wglMakeCurrent(m_hDC, m_hRC))
    {
        cerr << __FUNCTION__ << ": wglMakeCurrent failed." << endl;
        cleanup();
        return;
    }

    if (gl::preload() && gl::has("WGL_ARB_create_context"))
    {
        /*  Specify the OpenGL version.
         */

        int32_t major, minor;

        gl::glGetIntegerv(gl::GL_MAJOR_VERSION, &major);
        gl::glGetIntegerv(gl::GL_MINOR_VERSION, &minor);

        int32_t attrs[ ] = {
            gl::WGL_CONTEXT_MAJOR_VERSION_ARB, major,
            gl::WGL_CONTEXT_MINOR_VERSION_ARB, minor,
            gl::WGL_CONTEXT_FLAGS_ARB, 0,
            0
        };

        if (is_forward_compatible)
        {
            attrs[5] = gl::WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
        }

        HGLRC hRC = gl::wglCreateContextAttribsARB(m_hDC, nullptr, attrs);

        if (!hRC)
        {
            cerr << __FUNCTION__ << ": wglCreateContextAttribsARB failed." << endl;
            cleanup();
            return;
        }

        gl::wglMakeCurrent(nullptr, nullptr);
        gl::wglDeleteContext(m_hRC);

        m_hRC = hRC;

        if (!gl::wglMakeCurrent(m_hDC, m_hRC))
        {
            cerr << __FUNCTION__ << ": wglMakeCurrent failed." << endl;
            cleanup();
            return;
        }
    }

    if (!gl::init())
    {
        cerr << __FUNCTION__ << ": OpenGL initialization failed." << endl;
        cleanup();
        return;
    }
}

glcontext::~glcontext()
{
    cleanup();
}

void glcontext::swap_buffers()
{
    SwapBuffers(m_hDC);
}

void glcontext::cleanup()
{
    if (m_hDC)
    {
        if (m_hRC)
        {
            gl::wglMakeCurrent(m_hDC, nullptr);
            gl::wglDeleteContext(m_hRC);
        }

        if (m_window && m_window->get_native_handle())
        {
            ReleaseDC(m_window->get_native_handle(), m_hDC);
        }
    }

    gl::platform_cleanup();

    m_hDC = nullptr;
    m_hRC = nullptr;
}
