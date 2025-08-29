

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xxprintf.h"

/* The Point type */
typedef struct {
  double x;
  double y;
} Point;

/* The renderer callback for Point */
static int print_point(FILE *stream, const void *arg) {
  const Point *c = (const Point *) arg;
  return fprintf(stream, "%lf %lf", c->x, c->y);
}

int main(void) {
  Point point = {12345.6789, 3.141593};

  /* Register the callbacks for Point in the domain */
  xxprintf("BEFORE: %d\n >>'%'C'<< \n", 45678, &point);
  xxprintf_register_specifier('C', print_point, XXPRINT_ARG_POINTER);
  xxprintf(" AFTER: %d\n >>'%'C'<< \n", 45678, &point);

  /* Print the Point using the current locale (C). */
  xxprintf("|%'C|\n", &point);
  xxprintf("|%'14C|\n", &point);
  xxprintf("|%'-14.2C|\n", &point);
  xxprintf("|%'#C|\n", &point);
  xxprintf("|%'#14C|\n", &point);
  xxprintf("|%'#-14.2C|\n", &point);
  xxprintf("-------------\n");

  /* xxprintf to memory buffer */
  char buffer[2048] = {0};
  char *ptr = NULL;
  FILE *fp = NULL;

  // Test xxsprintf
  memset((void *) &buffer, 0, sizeof(buffer));
  xxsprintf(buffer, "xxsprintf |%'C| %d\n", &point, 1234);
  printf("%s\n", buffer);

  // Test xxsnprintf
  memset((void *) &buffer, 0, sizeof(buffer));
  xxsnprintf(buffer, sizeof(buffer), "xxsnprintf |%'C| %d\n", &point, 2345);
  printf("%s\n", buffer);

  // Test xxasprintf
  xxasprintf(&ptr, "xxasprintf |%'C| %d\n", &point, 3456);
  if (!ptr) err(1, "xxasprintf");
  printf("%s\n", ptr);
  free((void *) ptr);

  // Test xxfprintf
  memset((void *) &buffer, 0, sizeof(buffer));
  fp = fmemopen((void *) buffer, sizeof(buffer) - 1, "w");
  if (!fp) err(1, "fmemopen");
  xxfprintf(fp, "xxfprintf |%'C| %d\n", &point, 4567);
  fflush(fp);
  printf("%s\n", buffer);
  fclose(fp);

  // Test print_point directly
  memset((void *) &buffer, 0, sizeof(buffer));
  fp = fmemopen((void *) buffer, sizeof(buffer) - 1, "w");
  if (!fp) err(1, "fmemopen");
  print_point(fp, (const void *const *) &point);
  fflush(fp);
  printf("%s\n", buffer);
  fclose(fp);
  return 0;
}
