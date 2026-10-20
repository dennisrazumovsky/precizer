/* SPDX-FileCopyrightText: 2020 Thomas Mathys
   SPDX-License-Identifier: LGPL-2.1-or-later
   argp-standalone - standalone version of glibc's argp functions. */

/* Ensure we get the POSIX version of strerror_r.
   We do not try to use the GNU version if available.
   We'd have to be able to detect whether we have GNU or POSIX,
   and that is just hairy and not worth the trouble. */
#undef _GNU_SOURCE
#define _POSIX_C_SOURCE 200112L

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "argp-compat.h"

#if defined(HAVE_MEMPCPY) && !HAVE_MEMPCPY
void* argp_compat_mempcpy(void* out, const void* in, size_t n)
{
  memcpy(out, in, n);
  return (char*)out + n;
}
#endif

#if defined(HAVE_SLEEP) && !HAVE_SLEEP
#if defined(_WIN32)
void argp_compat_sleep(unsigned int seconds)
{
  for (; seconds; --seconds)
  {
    Sleep(1000);
  }
}
#else
#error No replacement for sleep() is available for the target platform
#endif
#endif

#if defined(HAVE_STRCASECMP) && !HAVE_STRCASECMP
#if defined(_WIN32)
int argp_compat_strcasecmp(const char* s1, const char* s2)
{
  return _stricmp(s1, s2);
}
#else
#error No replacement for strcasecmp() is available for the target platform
#endif
#endif

#if defined(HAVE_STRCHRNUL) && !HAVE_STRCHRNUL
const char* argp_compat_strchrnul(const char* s, int c)
{
  while (*s && (*s != c))
  {
    ++s;
  }
  return s;
}
#endif

#if defined (HAVE_STRNDUP) && !HAVE_STRNDUP
char* argp_compat_strndup(const char* s, size_t n)
{
  char* result;

  /* Find out how many chars to copy: entire string or n characters? */
  size_t s_len = strlen(s);
  if (s_len < n)
    n = s_len;

  /* Get memory, return 0 if insufficient memory. */
  result = malloc(n + 1);
  if (!result)
    return result;

  /* Copy (sub)string and terminate it */
  memcpy(result, s, n);
  result[n] = '\0';
  return result;
}
#endif

const char* argp_compat_strerror(int errnum, char buf[], size_t size)
{
#ifdef _GNU_SOURCE
#error _GNU_SOURCE must not be defined here
#endif

  if (size < 1)
  {
    return "";
  }

  /* Prefer strerror_s. It's the most sane API, actually. */
#if defined(HAVE_DECL_STRERROR_S) && HAVE_DECL_STRERROR_S
  strerror_s(buf, size, errnum);
  return buf;
#elif defined(HAVE_DECL_STRERROR_R) && HAVE_DECL_STRERROR_R

  if (!errnum)
  {
    return "No error";
  }

  int result = strerror_r(errnum, buf, size);

  /* Some implementations might not terminate the string
     if the buffer is too short. */
  buf[size - 1] = 0;

  /* Some implementations might not write to buf on error.
     Ensure buf always contains a message. */
  switch (result)
  {
    case 0:
      break;
    case EINVAL:
      snprintf(buf, size, "Unknown error %d", errnum);
      break;
    case ERANGE:
      snprintf(buf, size, "ERANGE from strerror_r");
      break;
    default:
      snprintf(buf, size, "Error %d from strerror_r", result);
      break;
  }

  return buf;
#else
  return strerror(errnum);
#endif
}
