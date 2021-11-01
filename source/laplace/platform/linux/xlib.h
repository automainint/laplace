/*  laplace/platform/xlib.h
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

#ifndef laplace_platform_linux_xlib_h
#define laplace_platform_linux_xlib_h

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#ifdef linux
#  undef linux
#endif

#ifdef None
#  undef None
#endif

#ifdef Bool
#  undef Bool
#endif

#endif
