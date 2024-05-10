#include <stdlib.h>
#include "r_context.c"

dllexport_t cafe_graphics_exports[] = {
    { "GetRefAPI", (void*)GetRefAPI },
    { NULL, NULL },
};

extern int setup_dll_funcs( void )
{
    return dll_register( "libref_soft.so", cafe_graphics_exports );
}