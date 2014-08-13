// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __OS_SYSTEM_H__
#define __OS_SYSTEM_H__

#if defined(_WIN32)
#if defined(_PT_KERNEL_)
#ifdef _PT_NDIS_
#else
#endif // _PT_NDIS_
#else
#endif // _PT_KERNEL_
#endif // _WIN32

#if defined(_UNIX_)
#if defined(_PT_KERNEL_)
#include "OS_KMLSystem.h"
#else
#endif // _PT_KERNEL_
#endif //_UNIX_

#endif // __OS_SYSTEM_H__
