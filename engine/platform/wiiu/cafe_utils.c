#include <math.h>
#include "imagelib.h"

#if XASH_WIIU
#include <vpad/input.h>
#include <coreinit/screen.h>
#include <coreinit/cache.h>
#include <whb/proc.h>
#include <whb/log_console.h>
#include <whb/log.h>
#include <whb/sdcard.h>
#include <coreinit/thread.h>
#include <coreinit/time.h>
#endif

/* Prepends t into s. Assumes s has enough space allocated
** for the combined string.
*/
void prepend(char* s, const char* t)
{
    size_t len = strlen(t);
    memmove(s + len, s, strlen(s) + 1);
    memcpy(s, t, len);
}