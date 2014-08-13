// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __OS_TYPES_H__
#define __OS_TYPES_H__

#if defined(WIN32)
#if defined(_PT_KERNEL_)
#ifdef _PT_NDIS_
#include "OS_KMNTypes.h" 
#else
#endif // _PT_NDIS_
#else
#include "OS_UMWTypes.h"
#endif // _PT_KERNEL_
#endif // _WIN32
#if defined (_UNIX_)
#if defined(_PT_KERNEL_)
#include "OS_KMLTypes.h"
#else
#include "OS_UMLTypes.h"
#endif // _PT_KERNEL_
#endif


#endif // __OS_TYPES_H__
