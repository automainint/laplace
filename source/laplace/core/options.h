/*  laplace/core/options.h
 *
 *      Laplace options.
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

#ifndef laplace_core_options_h
#define laplace_core_options_h

/*  Make sure all integer types are correct.
 */
static_assert(sizeof(signed char) == 1);
static_assert(sizeof(signed short) == 2);
static_assert(sizeof(signed int) == 4);
static_assert(sizeof(signed long long) == 8);
static_assert(sizeof(unsigned char) == 1);
static_assert(sizeof(unsigned short) == 2);
static_assert(sizeof(unsigned int) == 4);
static_assert(sizeof(unsigned long long) == 8);

/*  Enable verbose logging.
 */
//  #define LAPLACE_VERBOSE

/*  Disable safety checks.
 */
//  #define LAPLACE_UNSAFE

/*  Force to use POSIX sockets.
 */
//  #define LAPLACE_POSIX_SOCKETS

/*  Disable platform-dependent features.
 */
//  #define LAPLACE_PLATFORM_DUMMY

/*  Enable all warnings in MSVC.
 */
//  #define LAPLACE_MSVC_WARNINGS 1

/*  Windows preprocessor definitions.
 */
//  #define _WIN32 1
//  #define _WINDOWS 1

/*  Console application preprocessor definitions.
 */
//  #define _CONSOLE 1

#if defined(_MSC_VER) && !defined(LAPLACE_MSVC_WARNINGS)
#  pragma warning(disable : 4018 4100 4245 4250 4389 26812)
#endif

namespace laplace {
#ifdef LAPLACE_UNSAFE
  constexpr bool _unsafe = true;
#else
  constexpr bool _unsafe = false;
#endif
}

#endif
