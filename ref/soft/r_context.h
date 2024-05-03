#ifndef R_CONTEXT_H
#define R_CONTEXT_H

#include "r_local.h"

int EXPORT GetRefAPI( int version, ref_interface_t *funcs, ref_api_t *engfuncs, ref_globals_t *globals );

#endif