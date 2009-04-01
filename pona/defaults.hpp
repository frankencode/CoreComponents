/*
 * defaults.hpp -- library configuration
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#ifndef PONA_OPTIONS_HPP
#define PONA_OPTIONS_HPP

#ifndef PONA_DEFAULT_REF_POLICY
#ifdef NDEBUG
#define PONA_DEFAULT_REF_POLICY Pointer
#else
#define PONA_DEFAULT_REF_POLICY SetNull
#endif
#endif

#ifndef PONA_DEFAULT_BUF_CAPA
#define PONA_DEFAULT_BUF_CAPA (1 << 14)
#endif

#ifndef PONA_DEFAULT_ENDIAN
#define PONA_DEFAULT_ENDIAN LittleEndian
#endif

#ifndef PONA_VLC_CHUNKSIZE
#define PONA_VLC_CHUNKSIZE 4
#endif

#define PONA_UNDEF -1

#endif // PONA_OPTIONS_HPP
