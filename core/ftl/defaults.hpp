 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
