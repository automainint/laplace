/*  laplace/core/options.h
 *
 *      The Laplace options.
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

/*  Enable verbose logging.
 */
//#define LAPLACE_VERBOSE

/*  Force to use POSIX sockets.
 */
//#define LAPLACE_POSIX_SOCKETS

/*  Disable platform-dependent features.
 */
//#define LAPLACE_PLATFORM_DUMMY

#ifdef _MSC_VER
#  pragma warning(disable : 4245 4100 4250 4018)
#endif

#endif
