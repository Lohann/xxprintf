# Portable Extensible printf for MacOS and Linux

Portable Extensible printf allows adding new (user-defined) conversion specifiers **printf** for MacOS and Linux.

**MacOS** and **GLIBC** use different approaches to add user-defined conversion specifiers. GLIBC allows user-defined specifiers to modify the **printf** program-wide, while **MacOS** specifiers are scoped and you must use [xprintf(3)](https://www.manpagez.com/man/3/xprintf/) family methods instead normal printf.
- MacOS: https://www.manpagez.com/man/5/xprintf/
- GLIBC: http://www.gnu.org/software/libc/manual/html_node/Customizing-Printf.html

This project implements a tiny abstraction layer that simplifies the specifier registration and allows the same code to work in both systems using **xxprintf** family methods. Example:

```c
#include <stdio.h>
#include "xxprintf.h"

/* The Point type */
typedef struct {
  double x;
  double y;
} Point;

/* The renderer callback for Point */
static int print_point(FILE *stream, const void *arg) {
  const Point *c = (const Point *) arg;
  return fprintf(stream, "[%lf, %lf]", c->x, c->y);
}

int main(void) {
  Point point = {2.718282, 3.141593};

  // Register the specifier 'C' for `print_point`
  xxprintf_register_specifier('C', print_point, XXPRINT_ARG_POINTER);

  // output: "product of [2.718282, 3.141593] = 8.539736\n"
  xxprintf("product of %C = %d\n", &point, point.x * point.y);
}
```
