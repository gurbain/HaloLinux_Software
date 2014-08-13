
#include "OS_KMLGCCCompiler.h"

#if !( defined( _PT_64_ ) || defined( __LP64__ ) )

#include "asm/div64.h"

OS_UINT64 OS_DIV64( OS_UINT64 a, OS_UINT64 b )
{
    OS_UINT64 lReturn = a;
    do_div( lReturn, b );
    return lReturn;
}

OS_UINT64 OS_MOD64( OS_UINT64 a, OS_UINT64 b )
{
    OS_UINT64 lReturn = a;
    return do_div( lReturn, b );
}
 
#endif 


