#include "xxprintf.h"
#include <printf.h>

/*
 * Compatibility with compilers and environments that don't support compiler
 * feature checking function-like macros.
 */
#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

/* __cold marks code used for debugging or that is rarely taken
 * and tells the compiler to optimize for size and outline code.
 */
#if !defined __cold
#if __has_attribute(cold)
#define __cold __attribute__((__cold__))
#else
#define __cold
#endif
#endif

/*
 * __pure2 can be used for functions that are only a function of their scalar
 * arguments (meaning they can't dereference pointers).
 */
#if !defined __pure2
#if __has_attribute(__const__)
#define __pure2 __attribute__((__const__))
#else
#define __pure2
#endif
#endif

/*
 * Struct that stores the specifier function and it's arguments.
 */
typedef struct xxprintf_specifier_t {
  xxprintf_function *render;
  int argtype;
  int argsize;
} xxprintf_specifier_t;

/*
 * Lookup table that maps a specifier to `xxprintf_specifier_t`
 */
static xxprintf_specifier_t _specifiers[256] = {0};

/*
 * Lookup table that maps a specifier to `XXPRINT_ERROR_*`
 */
#define INV -XXPRINT_ERROR_INVALID_SPEC
#define RES -XXPRINT_ERROR_RESERVED_SPEC
static const char _specifiers_lut[256] = {
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, RES, INV, INV, RES, RES, RES, INV, RES, INV, INV, RES, RES, RES,
    RES, RES, INV, RES, RES, RES, RES, RES, RES, RES, RES, RES, RES, RES, RES,
    INV, INV, INV, INV, INV, RES, 'B', 'C', 'D', RES, RES, RES, 'H', 'I', 'J',
    'K', RES, 'M', 'N', 'O', 'P', RES, 'R', 'S', 'T', 'U', 'V', 'W', RES, 'Y',
    'Z', INV, INV, INV, INV, RES, INV, RES, 'b', RES, RES, RES, RES, RES, RES,
    RES, RES, 'k', RES, 'm', 'n', RES, RES, RES, 'r', RES, RES, RES, RES, 'w',
    RES, 'y', RES, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV, INV,
    INV};
#undef INV
#undef RES

/*
 * Maps an `XXPRINT_ARG_*` to it's size.
 * Returns -1 if `argtype` is invalid.
 */
static inline int get_argsize(int argtype) __pure2;
static inline int get_argsize(int argtype) {
  switch (argtype) {
    case XXPRINT_ARG_INT:
      return sizeof(int);
    case XXPRINT_ARG_CHAR:
      return sizeof(char);
    case XXPRINT_ARG_WCHAR:
      return sizeof(wchar_t);
    case XXPRINT_ARG_STRING:
    case XXPRINT_ARG_WSTRING:
    case XXPRINT_ARG_POINTER:
      return sizeof(void *);
    case XXPRINT_ARG_FLOAT:
      return sizeof(float);
    case XXPRINT_ARG_DOUBLE:
      return sizeof(double);
    default:
      return -1;
  }
}

/***********
 * APPLE
 ***********/
#if defined(__APPLE__) || defined(__MACH__)
#include <limits.h> /* INT_MAX */

static printf_domain_t apple_domain = NULL;

int apple_init_domain() __cold;
int apple_init_domain() {
  apple_domain = new_printf_domain();
  if (apple_domain == NULL) {
    return XXPRINT_ERROR_OUT_OF_MEMORY;
  }
  return 0;
}

static int apple_printf_render(FILE *stream, const struct printf_info *info,
                               const void *const *args) {
  xxprintf_specifier_t *spec = (xxprintf_specifier_t *) info->context;
  if (spec == NULL) return -1;
  return spec->render(stream, *((const void **) args[0]));
}

static int apple_printf_arginfo(const struct printf_info *info, size_t n,
                                int *argtypes) {
  xxprintf_specifier_t *spec = (xxprintf_specifier_t *) info->context;
  if (spec == NULL) return -1;
  if (n > 0) argtypes[0] = 1 << spec->argtype;
  return 1;
}

int xxprintf_register_specifier(char spec, xxprintf_function *render,
                                int argtype) {
  const unsigned char index = ((int) spec) & 0xff;
  int res = (int) _specifiers_lut[index];
  if (res < 0) return -res;
  res = get_argsize(argtype);
  if (!render || res < 0) return XXPRINT_ERROR_MISUSE;
  if (!apple_domain) apple_init_domain();

  _specifiers[index].render = render;
  _specifiers[index].argtype = argtype;
  _specifiers[index].argsize = res;
  if (register_printf_domain_function(
          apple_domain, (int) spec, &apple_printf_render, &apple_printf_arginfo,
          (void *) &_specifiers[index])) {
    return XXPRINT_ERROR_OUT_OF_MEMORY;
  }
  return XXPRINT_OK;
}

int xxvprintf(const char *__restrict fmt, va_list ap) {
  if (!apple_domain) apple_init_domain();
  return vxprintf(apple_domain, NULL, fmt, ap);
}
int xxvfprintf(FILE *__restrict stream, const char *__restrict fmt,
               va_list ap) {
  if (!apple_domain) apple_init_domain();
  return vfxprintf(stream, apple_domain, NULL, fmt, ap);
}
int xxvsnprintf(char *__restrict str, size_t size, const char *__restrict fmt,
                va_list ap) {
  if (!apple_domain) apple_init_domain();
  return vsxprintf(str, size, apple_domain, NULL, fmt, ap);
}
int xxvsprintf(char *str, const char *fmt, va_list ap) {
  return xxvsnprintf(str, INT_MAX - 1, fmt, ap);
}
int xxvasnprintf(char **__restrict strp, const char *__restrict fmt,
                 va_list ap) {
  if (!apple_domain) apple_init_domain();
  return vasxprintf(strp, apple_domain, NULL, fmt, ap);
}

#elif defined(__linux__) || defined(__unix__)
/***********
 * LINUX
 ***********/
static int gnu_printf_render(FILE *fp, const struct printf_info *info,
                             const void *const *args) {
  if (info->spec > 255) return -1;
  int index = ((int) info->spec) & 0xff;
  xxprintf_specifier_t *spec = (xxprintf_specifier_t *) &_specifiers[index];
  return spec->render(fp, *((const void **) args[0]));
}

static int gnu_printf_arginfo(const struct printf_info *info, size_t n,
                              int *argtypes, int *size) {
  if (info->spec > 255) return -1;
  int index = ((int) info->spec) & 0xff;
  xxprintf_specifier_t *spec = (xxprintf_specifier_t *) &_specifiers[index];
  if (n > 0) {
    argtypes[0] = spec->argtype;
    size[0] = spec->argsize;
  }
  return 1;
}

int xxprintf_register_specifier(char spec, xxprintf_function *render,
                                int argtype) {
  const unsigned char index = ((int) spec) & 0xff;
  int res = (int) _specifiers_lut[index];
  if (res < 0) return -res;
  res = get_argsize(argtype);
  if (!render || res < 0) return XXPRINT_ERROR_MISUSE;
  _specifiers[index].render = render;
  _specifiers[index].argtype = argtype;
  _specifiers[index].argsize = res;
  if (register_printf_specifier((int) spec, &gnu_printf_render,
                                &gnu_printf_arginfo) < 0) {
    return XXPRINT_ERROR_OUT_OF_MEMORY;
  }
  return XXPRINT_OK;
}

#define xxvprintf vprintf
#define xxvfprintf vfprintf
#define xxvsnprintf vsnprintf
#define xxvsprintf vsprintf
#define xxvasnprintf vasprintf

#else /* defined(__linux__) || defined(__unix__) */
#error "unsupported OS"
#endif /* defined(__APPLE__) || defined(__MACH__) */

int xxprintf(const char *fmt, ...) {
  int res;
  va_list ap;
  va_start(ap, fmt);
  res = xxvprintf(fmt, ap);
  va_end(ap);
  return res;
}

int xxfprintf(FILE *__restrict stream, const char *__restrict fmt, ...) {
  int res;
  va_list ap;
  va_start(ap, fmt);
  res = xxvfprintf(stream, fmt, ap);
  va_end(ap);
  return res;
}

int xxsprintf(char *str, const char *fmt, ...) {
  int res;
  va_list ap;
  va_start(ap, fmt);
  res = xxvsprintf(str, fmt, ap);
  va_end(ap);
  return res;
}

int xxsnprintf(char *str, size_t size, const char *fmt, ...) {
  int res;
  va_list ap;
  va_start(ap, fmt);
  res = xxvsnprintf(str, size, fmt, ap);
  va_end(ap);
  return res;
}

int xxasprintf(char **__restrict strp, const char *__restrict fmt, ...) {
  int res;
  va_list ap;
  va_start(ap, fmt);
  res = xxvasnprintf(strp, fmt, ap);
  va_end(ap);
  return res;
}
