/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

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
