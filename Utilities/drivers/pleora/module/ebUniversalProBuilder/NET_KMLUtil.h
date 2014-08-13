// *****************************************************************************
//
//     Copyright (c) 2011, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __NET_KMLUTIL_H__
#define __NET_KMLUTIL_H__

#include "OS_Compiler.h"
#include "OS_Types.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

///
/// \brief Return the size of a packet
///
/// \param aData Data Pointer passed by the manager ( in this OS -> sk_buff )
///
/// \return The size of the packet
///
OS_EXTERN OS_UINT32 NET_GetPacketSize( OS_VOID* aData );

/// 
/// \brief Copy NET_BUFFER to a continuous memory location
/// 
/// \param aData Data Pointer passed by the manager ( in this OS -> sk_buff )
/// \param aOffset Offset in the packet to copy from
/// \param aDest Destination memory area pre-allocated
/// \param aSize Size of the destination
///
/// \return The current number of bytes copied
///
OS_EXTERN OS_VOID NET_Copy( OS_VOID* aData, OS_UINT32 aOffset, OS_UINT8* aDest, OS_UINT32 aSize );

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __NET_KMLUTIL_H__
