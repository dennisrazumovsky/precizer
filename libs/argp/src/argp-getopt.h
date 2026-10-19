/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* Declarations for getopt.
   Copyright (C) 1989-2026 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Unlike the bulk of the getopt implementation, this file is NOT part
   of gnulib; gnulib also has a getopt.h but it is different.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#ifndef _GETOPT_H
#define _GETOPT_H 1

/* argp-standalone: include <ctype.h> to get glibc's <feature.h> if it exists.
   This will get us the definition of __THROW.  */
/* If __GNU_LIBRARY__ is not already defined, either we are being used
   standalone, or this is the first header included in the source file.
   If we are being used with glibc, we need to include <features.h>, but
   that does not exist if we are standalone.  So: if __GNU_LIBRARY__ is
   not defined, include <ctype.h>, which will pull in <features.h> for us
   if it's from glibc.  (Why ctype.h?  It's guaranteed to exist and it
   doesn't flood the namespace with stuff the way some other headers do.)  */
#if !defined __GNU_LIBRARY__
# include <ctype.h>
#endif

/* argp-standalone: define __THROW. Taken from older version of <getopt.h> */
#ifndef __THROW
# ifndef __GNUC_PREREQ
#  define __GNUC_PREREQ(maj, min) (0)
# endif
# if defined __cplusplus && __GNUC_PREREQ (2,8)
#  define __THROW	throw ()
# else
#  define __THROW
# endif
#endif

/* The type of the 'argv' argument to getopt_long and getopt_long_only
   is properly 'char **', since both functions may write to the array
   (in order to move all the options to the beginning).  However, for
   compatibility with old versions of LSB, glibc has to use 'char *const *'
   instead.  */
#ifndef __getopt_argv_const
# define __getopt_argv_const const
#endif

/* argp-standalone: include our own renamed and patched headers from glibc. */
#include "argp-getopt_core.h"
#include "argp-getopt_ext.h"

#endif /* getopt.h */
