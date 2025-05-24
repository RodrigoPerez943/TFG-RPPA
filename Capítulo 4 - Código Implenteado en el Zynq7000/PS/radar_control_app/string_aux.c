#include <stdlib.h>
#include <string.h>
#include "string_aux.h"

size_t strlcpy(char *dest, const char *src, size_t size)
{
    size_t ret = strlen(src);

    if (size) {
          size_t len = (ret >= size) ? size : ret;
          __builtin_memcpy(dest, src, len);
          dest[len] = '\0';
    }
    return ret;
}
