#ifndef DH_UTILS_H
#define DH_UTILS_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

    void *file_contents(const char *, size_t *);

#ifdef __cplusplus
}
#endif

#define ARRLEN(a)   (sizeof(a) / sizeof(a[0]))

#endif
