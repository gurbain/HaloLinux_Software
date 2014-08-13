// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __OS_INIT_H__
#define __OS_INIT_H__

#if defined(WIN32)
#if defined( _PT_KERNEL_ )
#ifdef _PT_NDIS_
#include "OS_KMNInit.h"    
#else
#endif // _PT_NDIS_
#else

#include "OS_UMWInit.h"
#endif // _PT_KERNEL_
#endif // _WIN32

#if defined(_UNIX_)
#if defined( _PT_KERNEL_ )
#include "OS_KMLInit.h"
#else
#include "OS_UMLInit.h"
#endif // _PT_KERNEL_
#endif //_UNIX_

#endif // __OS_INIT_H__
