// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __OS_KMLMEMORYMAP_H__
#define __OS_KMLMEMORYMAP_H__

#include "OS_Compiler.h"
#include "OS_Misc.h"
#include "OS_Types.h"

typedef struct 
{
    unsigned long  PageCount;
    OS_VOID**      PageList;
    unsigned long  OffsetPage;
    OS_VOID*       KernelAddress;
	OS_VOID*       UserAddress;
} OS_MEMORYMAP;  

///
/// Get the Kernel space address of a map structure
///
/// \param aMap OS_MEMORYMAP Pointer to an functional memory map structure
///
/// \return The pointer to the kernel field
#define OS_MAPGETKERNELADDRESS( aMap )                                         \
    ( ( aMap )->KernelAddress )

///
/// Get the User space address of a map structure
///
/// \param aMap OS_MEMORYMAP Pointer to an functional memory map structure
///
/// \return The pointer to the User field
#define OS_MAPGETUSERADDRESS( aMap )                                           \
    ( ( aMap )->UserAddress )

OS_EXTERN OS_VOID* OS_MapCreate( OS_DRV_CONTEXT* aContext, OS_MEMORYMAP* aMap, OS_VOID* aUserAddress, OS_UINT32 aSize );
OS_EXTERN OS_VOID* OS_MapDelete( OS_MEMORYMAP* aMap );

#endif // __OS_KMLMEMORYMAP_H__
