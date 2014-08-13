// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __OS_KMLGCCCOMPILER_H__
#define __OS_KMLGCCCOMPILER_H__

#include "OS_Types.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define OS_EXTERN           extern

#define OS_INLINE           static inline

#define OS_EXPORTABLE       OS_EXTERN

#if ( defined( _PT_64_ ) || defined( __LP64__ ) )

#define OS_DEFAULT_PACK     16

OS_INLINE OS_UINT64 OS_DIV64( OS_UINT64 a, OS_UINT64 b )
{
    return a / b;
}
OS_INLINE OS_UINT64 OS_MOD64( OS_UINT64 a, OS_UINT64 b )
{
    return a % b;
}
#else

#define OS_DEFAULT_PACK     8

OS_EXTERN OS_UINT64 OS_DIV64( OS_UINT64 a, OS_UINT64 b );
OS_EXTERN OS_UINT64 OS_MOD64( OS_UINT64 a, OS_UINT64 b );
#endif 

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __OS_KMLGCCCOMPILER_H__
