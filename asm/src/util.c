#include "util.h"

#include <ctype.h>

bool UtilStrCase(const char *s1, const char *s2) {
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;

    while (*p1 != '\0' && *p2 != '\0') {
        unsigned char c1 = tolower(*p1);
        unsigned char c2 = tolower(*p2);

        if (c1 != c2) { return c1 - c2; }
        p1++;
        p2++;
    }

    return tolower(*p1) - tolower(*p2);
}
