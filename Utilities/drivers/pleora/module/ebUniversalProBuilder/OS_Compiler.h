// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __OS_COMPILER_H__
#define __OS_COMPILER_H__

#if defined(_WIN32)
#if defined(_PT_KERNEL_)
#ifdef _PT_NDIS_
#include "OS_KMNCompiler.h"  
#else
#endif // _PT_NDIS_
#else
#include "OS_UMWCompiler.h"
#endif // _PT_KERNEL_
#endif // _WIN32

#if defined(_UNIX_)
#if defined(_PT_KERNEL_)
#include "OS_KMLGCCCompiler.h"
#else
#include "OS_UMLGCCCompiler.h"
#endif // _PT_KERNEL_
#endif

#endif // __OS_COMPILER_H__
