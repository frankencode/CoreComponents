/*
 * defaults.hpp -- library configuration
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_DEFAULTS_HPP
#define FTL_DEFAULTS_HPP

#ifndef FTL_DEFAULT_REF_POLICY
#ifdef NDEBUG
#define FTL_DEFAULT_REF_POLICY Pointer
#else
#define FTL_DEFAULT_REF_POLICY SetNull
#endif
#endif

// #define FTL_COREMUTEX_USE_POSIX_SEMAPHORES
// #define FTL_COREMUTEX_USE_MACOS_SPIN_LOCKS
// #define FTL_COREMUTEX_USE_POSIX_SEMAPHORES

// #define FTL_REF_POLICY_RACE_DETECTION

#ifndef FTL_DEFAULT_BUF_CAPA
#define FTL_DEFAULT_BUF_CAPA (1 << 14)
#endif

#ifndef FTL_DEFAULT_ENDIAN
#define FTL_DEFAULT_ENDIAN LittleEndian
#endif

#ifndef FTL_VLC_CHUNKSIZE
#define FTL_VLC_CHUNKSIZE 4
#endif

#ifndef FTL_DEFAULT_BACKLOG
#define FTL_DEFAULT_BACKLOG 8
#endif

#define FTL_UNDEF -1

#endif // FTL_DEFAULTS_HPP
