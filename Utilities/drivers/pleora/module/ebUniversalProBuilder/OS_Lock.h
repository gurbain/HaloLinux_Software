// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __OS_LOCK_H__
#define __OS_LOCK_H__

#if defined(WIN32)
#if defined(_PT_KERNEL_)
#ifdef _PT_NDIS_
#include "OS_KMNLock.h"  
#else
#endif // _PT_NDIS_
#else
#include "OS_UMWLock.h"
#endif // _PT_KERNEL_
#endif // _WIN32

#if defined(_UNIX_)
#if defined(_PT_KERNEL_)
#include "OS_KMLLock.h"
#else
#include "OS_UMLLock.h"
#endif // _PT_KERNEL_
#endif

#endif // __OS_LOCK_H__
