#ifndef __STRING_AUX_H
#define __STRING_AUX_H

/* This is for including "special" functions such as strlcpy.
 * This is not the best option for including it, but it is fast
 */

size_t strlcpy(char *dest, const char *src, size_t size);

#endif
