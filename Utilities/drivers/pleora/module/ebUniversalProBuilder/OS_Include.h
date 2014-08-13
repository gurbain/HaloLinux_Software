// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __OS_INCLUDE_H__
#define __OS_INCLUDE_H__

#if defined(WIN32)
#if defined( _PT_KERNEL_ )
#ifdef _PT_NDIS_
#include "OS_KMNInclude.h"    
#else
#endif // _PT_NDIS_
#else
#include "OS_UMWInclude.h"
#endif // _PT_KERNEL_
#endif // _WIN32

#if defined(_UNIX_)
#if defined( _PT_KERNEL_ )
#include "OS_KMLInclude.h" 
#else
#include "OS_UMLInclude.h"
#endif //_PT_KERNEL_
#endif // _UNIX_

#endif // __OS_INCLUDE_H__
