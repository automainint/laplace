/*  laplace/platform/win32/win.predef.h
 *
 *      Win32 minimum declarations.
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

#ifndef __laplace__windows_header__
#define __laplace__windows_header__

#include <cstdint>

using BOOL   = int32_t;
using WPARAM = uint64_t;
using LPARAM = int64_t;

using HWND      = struct HWND__ *;
using HINSTANCE = struct HINSTANCE__ *;
using HICON     = struct HICON__ *;
using HDROP     = struct HDROP__ *;
using HRAWINPUT = struct HRAWINPUT__ *;
using HDC       = struct HDC__ *;
using HGLRC     = struct HGLRC__ *;
using HMODULE   = HINSTANCE;

using LPGLYPHMETRICSFLOAT    = struct _GLYPHMETRICSFLOAT *;
using LPLAYERPLANEDESCRIPTOR = struct tagLAYERPLANEDESCRIPTOR *;

#endif
