// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __OS_TIMER_H__
#define __OS_TIMER_H__

#if defined(_WIN32)
#if defined(_PT_KERNEL_)
#ifdef _PT_NDIS_
#include "OS_KMNTimer.h" 
#else
#endif // _PT_NDIS_
#else
#include "OS_UMWTimer.h"
#endif // _PT_KERNEL_
#endif // _WIN32
#if defined (_UNIX_)
#if defined(_PT_KERNEL_)
#include "OS_KMLTimer.h"
#else
#include "OS_UMLTimer.h"
#endif // _PT_KERNEL_
#endif

#endif // __OS_TIMER_H__
