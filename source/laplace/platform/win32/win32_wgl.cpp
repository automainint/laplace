/*  laplace/platform/win32/win32_wgl.cpp
 *
 *      WGL functions loading from OpenGL DLL.
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

#include "wgl.h"
#include "../gldef.h"
#include <iostream>

#define LOAD(a) \
    if (gl::a = reinterpret_cast<pfn_##a>(GetProcAddress(opengl_dll, #a));  !gl::a) \
    { \
        wcerr << L"Unable to get " << L#a << L" function from " << opengl_path << L"." << endl; \
        status = false; \
    }

namespace laplace::gl
{
    extern std::vector<std::string> extensions;
}

using namespace laplace;
using namespace std;

HMODULE gl::opengl_dll = nullptr;

gl::pfn_wglCopyContext              gl::wglCopyContext = nullptr;
gl::pfn_wglCreateContext            gl::wglCreateContext = nullptr;
gl::pfn_wglCreateLayerContext       gl::wglCreateLayerContext = nullptr;
gl::pfn_wglDeleteContext            gl::wglDeleteContext = nullptr;
gl::pfn_wglGetCurrentContext        gl::wglGetCurrentContext = nullptr;
gl::pfn_wglGetCurrentDC             gl::wglGetCurrentDC = nullptr;
gl::pfn_wglGetProcAddress           gl::wglGetProcAddress = nullptr;
gl::pfn_wglMakeCurrent              gl::wglMakeCurrent = nullptr;
gl::pfn_wglShareLists               gl::wglShareLists = nullptr;
gl::pfn_wglUseFontBitmapsA          gl::wglUseFontBitmapsA = nullptr;
gl::pfn_wglUseFontBitmapsW          gl::wglUseFontBitmapsW = nullptr;
gl::pfn_wglUseFontOutlinesA         gl::wglUseFontOutlinesA = nullptr;
gl::pfn_wglUseFontOutlinesW         gl::wglUseFontOutlinesW = nullptr;
gl::pfn_wglDescribeLayerPlane       gl::wglDescribeLayerPlane = nullptr;
gl::pfn_wglSetLayerPaletteEntries   gl::wglSetLayerPaletteEntries = nullptr;
gl::pfn_wglGetLayerPaletteEntries   gl::wglGetLayerPaletteEntries = nullptr;
gl::pfn_wglRealizeLayerPalette      gl::wglRealizeLayerPalette = nullptr;
gl::pfn_wglSwapLayerBuffers         gl::wglSwapLayerBuffers = nullptr;
gl::pfn_wglCreateContextAttribsARB  gl::wglCreateContextAttribsARB = nullptr;

auto gl::platform_init() -> bool
{
    if (opengl_dll)
    {
        /*  Already done.
         */
        return true;
    }

    if (opengl_dll = LoadLibraryW(opengl_path); !opengl_dll)
    {
        wcerr   << L"Unable to load OpenGL dynamic-link library from "
                << opengl_path << L"." << endl;
        return false;
    }

    bool status = true;

    LOAD(wglCopyContext);
    LOAD(wglCreateContext);
    LOAD(wglCreateLayerContext);
    LOAD(wglDeleteContext);
    LOAD(wglGetCurrentContext);
    LOAD(wglGetCurrentDC);
    LOAD(wglGetProcAddress);
    LOAD(wglMakeCurrent);
    LOAD(wglShareLists);
    LOAD(wglUseFontBitmapsA);
    LOAD(wglUseFontBitmapsW);
    LOAD(wglUseFontOutlinesA);
    LOAD(wglUseFontOutlinesW);
    LOAD(wglDescribeLayerPlane);
    LOAD(wglSetLayerPaletteEntries);
    LOAD(wglGetLayerPaletteEntries);
    LOAD(wglRealizeLayerPalette);
    LOAD(wglSwapLayerBuffers);

    return status;
}

auto gl::preload() -> bool
{
    bool status = true;

    if (!glGetIntegerv)
    {
        glGetIntegerv =
            reinterpret_cast<pfn_glGetIntegerv>(
                get_proc_address("glGetIntegerv")
            );

        if (!glGetIntegerv)
        {
            cerr << "Unable to get glGetIntegerv function." << endl;
            status = false;
        }
    }

    if (!glGetStringi)
    {
        glGetStringi =
            reinterpret_cast<pfn_glGetStringi>(
                get_proc_address("glGetStringi")
            );

        if (!glGetStringi)
        {
            cerr << "Unable to get glGetStringi function." << endl;
            status = false;
        }
    }

    if (glGetIntegerv && glGetStringi)
    {
        int32_t num_extensions = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);

        gl::extensions.reserve(num_extensions);

        for (int32_t i = 0; i < num_extensions; i++)
        {
            auto p = reinterpret_cast<const char *>(glGetStringi(GL_EXTENSIONS, i));

            if (p)
            {
                auto name = string(p);

                gl::extensions.emplace(
                    lower_bound(gl::extensions.begin(), gl::extensions.end(), name),
                    name);
            }
        }
    }

    if (has("WGL_ARB_create_context"))
    {
        wglCreateContextAttribsARB =
            reinterpret_cast<pfn_wglCreateContextAttribsARB>(
                get_proc_address("wglCreateContextAttribsARB")
            );

        if (!wglCreateContextAttribsARB)
        {
            cerr << "Unable to get wglCreateContextAttribsARB function." << endl;
            status = false;
        }
    }

    return status;
}

void gl::platform_cleanup()
{
    if (opengl_dll)
    {
        FreeLibrary(opengl_dll);
        opengl_dll = nullptr;
    }
}

auto gl::get_proc_address(const char *name) -> gl::ptr_function
{
    if (gl::wglGetProcAddress)
    {
        if (auto p = gl::wglGetProcAddress(name); p)
            return p;
    }

    return reinterpret_cast<ptr_function>(GetProcAddress(opengl_dll, name));
}
