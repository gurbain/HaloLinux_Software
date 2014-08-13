// *****************************************************************************
//
//     Copyright (c) 2011, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __NET_UTIL_H__
#define __NET_UTIL_H__

#if defined(_WIN32)
#if defined(_PT_KERNEL_)
#ifdef _PT_NDIS_
#include "NET_KMNUtil.h"    
#else
#endif // _PT_NDIS_
#else
#include "NET_UMUtil.h"
#endif // _PT_KERNEL_
#endif // _WIN32

#if defined(_UNIX_)
#if defined(_PT_KERNEL_)
#include "NET_KMLUtil.h"
#else
#include "NET_UMUtil.h"
#endif // _PT_KERNEL_
#endif // _UNIX_


#endif // __NET_UTIL_H__
