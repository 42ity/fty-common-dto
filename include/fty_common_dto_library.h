/*  =========================================================================
    fty-common-dto - generated layer of public API

    Copyright (C) 2014 - 2018 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
    =========================================================================
*/

#ifndef FTY_COMMON_DTO_LIBRARY_H_INCLUDED
#define FTY_COMMON_DTO_LIBRARY_H_INCLUDED

//  Set up environment for the application

//  External dependencies
#include <sodium.h>
#include <cxxtools/allocator.h>
#include <fty_common_messagebus.h>

//  FTY_COMMON_DTO version macros for compile-time API detection
#define FTY_COMMON_DTO_VERSION_MAJOR 1
#define FTY_COMMON_DTO_VERSION_MINOR 0
#define FTY_COMMON_DTO_VERSION_PATCH 0

#define FTY_COMMON_DTO_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define FTY_COMMON_DTO_VERSION \
    FTY_COMMON_DTO_MAKE_VERSION(FTY_COMMON_DTO_VERSION_MAJOR, FTY_COMMON_DTO_VERSION_MINOR, FTY_COMMON_DTO_VERSION_PATCH)

// czmq_prelude.h bits
#if !defined (__WINDOWS__)
#   if (defined WIN32 || defined _WIN32 || defined WINDOWS || defined _WINDOWS)
#       undef __WINDOWS__
#       define __WINDOWS__
#   endif
#endif

// Windows MSVS doesn't have stdbool
#if (defined (_MSC_VER) && !defined (true))
#   if (!defined (__cplusplus) && (!defined (true)))
#       define true 1
#       define false 0
        typedef char bool;
#   endif
#else
#   include <stdbool.h>
#endif
// czmq_prelude.h bits

#if defined (__WINDOWS__)
#   if defined FTY_COMMON_DTO_STATIC
#       define FTY_COMMON_DTO_EXPORT
#   elif defined FTY_COMMON_DTO_INTERNAL_BUILD
#       if defined DLL_EXPORT
#           define FTY_COMMON_DTO_EXPORT __declspec(dllexport)
#       else
#           define FTY_COMMON_DTO_EXPORT
#       endif
#   elif defined FTY_COMMON_DTO_EXPORTS
#       define FTY_COMMON_DTO_EXPORT __declspec(dllexport)
#   else
#       define FTY_COMMON_DTO_EXPORT __declspec(dllimport)
#   endif
#   define FTY_COMMON_DTO_PRIVATE
#elif defined (__CYGWIN__)
#   define FTY_COMMON_DTO_EXPORT
#   define FTY_COMMON_DTO_PRIVATE
#else
#   if (defined __GNUC__ && __GNUC__ >= 4) || defined __INTEL_COMPILER
#       define FTY_COMMON_DTO_PRIVATE __attribute__ ((visibility ("hidden")))
#       define FTY_COMMON_DTO_EXPORT __attribute__ ((visibility ("default")))
#   else
#       define FTY_COMMON_DTO_PRIVATE
#       define FTY_COMMON_DTO_EXPORT
#   endif
#endif

//  Project has no stable classes, so we build the draft API
#undef  FTY_COMMON_DTO_BUILD_DRAFT_API
#define FTY_COMMON_DTO_BUILD_DRAFT_API

//  Opaque class structures to allow forward references
//  These classes are stable or legacy and built in all releases
//  Draft classes are by default not built in stable releases
#ifdef FTY_COMMON_DTO_BUILD_DRAFT_API
typedef struct _fty_srr_dto_t fty_srr_dto_t;
#define FTY_SRR_DTO_T_DEFINED
typedef struct _fty_config_dto_t fty_config_dto_t;
#define FTY_CONFIG_DTO_T_DEFINED
#endif // FTY_COMMON_DTO_BUILD_DRAFT_API


//  Public classes, each with its own header file
#ifdef FTY_COMMON_DTO_BUILD_DRAFT_API
#include "fty_srr_dto.h"
#include "fty_config_dto.h"
#endif // FTY_COMMON_DTO_BUILD_DRAFT_API

#ifdef FTY_COMMON_DTO_BUILD_DRAFT_API

#ifdef __cplusplus
extern "C" {
#endif

//  Self test for private classes
FTY_COMMON_DTO_EXPORT void
    fty_common_dto_private_selftest (bool verbose, const char *subtest);

#ifdef __cplusplus
}
#endif
#endif // FTY_COMMON_DTO_BUILD_DRAFT_API

#endif
/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
*/
