/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "options.h"

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
