/* SPDX-License-Identifier: MIT */
/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Lohann Paterno Coutinho Ferreira
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _XXPRINTF_H_
#define _XXPRINTF_H_

#include <stdarg.h> /* __restrict */
#include <stddef.h> /* size_t */
#include <stdio.h>  /* FILE */

/* argument types, used to register a specifier */
#define XXPRINT_ARG_INT 0
#define XXPRINT_ARG_CHAR 1
#define XXPRINT_ARG_WCHAR 2
#define XXPRINT_ARG_STRING 3
#define XXPRINT_ARG_WSTRING 4
#define XXPRINT_ARG_POINTER 5
#define XXPRINT_ARG_FLOAT 6
#define XXPRINT_ARG_DOUBLE 7

#ifdef __cplusplus
extern "C" {
#endif

typedef int xxprintf_function(FILE *stream, const void *arg);

/* Possible status returned by `xxprintf_register_specifier` */
#define XXPRINT_OK 0
#define XXPRINT_ERROR_OUT_OF_MEMORY 1
#define XXPRINT_ERROR_INVALID_SPEC 2
#define XXPRINT_ERROR_RESERVED_SPEC 3
#define XXPRINT_ERROR_MISUSE 4
#define XXPRINT_ERROR_UNKNOWN 255

/*
 * Creates a new customizable `xxprint` specificier
 * On Linux it uses `register_printf_specifier`.
 * On MacOS it uses `register_printf_domain_function`.
 */
int xxprintf_register_specifier(char spec, xxprintf_function *render,
                                int argtype);

int xxprintf(const char *__restrict fmt, ...);
int xxvprintf(const char *__restrict fmt, va_list ap);

int xxfprintf(FILE *__restrict stream, const char *__restrict fmt, ...);
int xxvfprintf(FILE *__restrict stream, const char *__restrict fmt, va_list ap);

int xxsprintf(char *__restrict str, const char *__restrict fmt, ...);
int xxvsprintf(char *__restrict str, const char *__restrict fmt, va_list ap);

int xxsnprintf(char *__restrict str, size_t size, const char *__restrict fmt,
               ...);
int xxvsnprintf(char *__restrict str, size_t size, const char *__restrict fmt,
                va_list ap);

int xxasprintf(char **__restrict strp, const char *__restrict fmt, ...);
int xxvasnprintf(char **__restrict strp, const char *__restrict fmt,
                 va_list ap);

#ifdef __cplusplus
}
#endif

#endif /* _XXPRINTF_H_ */
