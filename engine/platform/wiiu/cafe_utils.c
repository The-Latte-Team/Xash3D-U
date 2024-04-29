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

char *sdCard;

/* Prepends t into s. Assumes s has enough space allocated
** for the combined string.
*/
void prepend(char* s, const char* t)
{
    size_t len = strlen(t);
    memmove(s + len, s, strlen(s) + 1);
    memcpy(s, t, len);
}

/*
 *      Remove given section from string. Negative len means remove
 *      everything up to the end.
 */
int str_cut(char *str, int begin, int len)
{
    int l = strlen(str);

    if (len < 0) len = l - begin;
    if (begin + len > l) len = l - begin;
    memmove(str + begin, str + begin + len, l - len + 1);

    return len;
}

char *GetSDCardPath()
{
    if (!WHBMountSdCard())
	    return NULL;

    sdCard = WHBGetSdCardMountPath();

    if (sdCard == NULL)
        return NULL;
	strcat(sdCard, "/wiiu/apps/xash3DU/valve/");

    return sdCard;
}