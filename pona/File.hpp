/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstractions Library.
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

#ifndef PONA_FILE_HPP
#define PONA_FILE_HPP

#include "Atoms.hpp"
#include "String.hpp"
#include "SystemStream.hpp"

namespace pona
{

class File: public SystemStream
{
public:
	typedef int64_t Offset;
	
	enum AccessFlags
	{
		readable = 4,
		writable = 2,
		executable = 1
	};
	
	enum CreateFlags
	{
		userRead   = 0400,
		userWrite  = 0200,
		userExec   = 0100,
		groupRead  = 0040,
		groupWrite = 0020,
		groupExec  = 0010,
		otherRead  = 0004,
		otherWrite = 0002,
		otherExec  = 0001
	};

	enum SeekMethod
	{
		seekBegin  = 1,
		seekCurrent = 2,
		seekEnd  = 3
	};

	enum StdStream
	{
		stdInput = 0,
		stdOutput = 1,
		stdError = 2
	};

	File(String path);
	File(int stdStream);
	~File();

	String path() const;
	int openFlags() const;

	bool access(int flags) const;
	bool exists() const;
	void create(int mask = 0644);
	void remove();

	void open(int flags = readable|writable);

	Offset seek(Offset distance, int method = seekBegin);
	void seekSet(Offset distance);
	void seekMove(Offset distance);
	Offset seekTell();
	Offset size();

	void sync();
	void dataSync();

private:
	String path_;
	int openFlags_;

#ifdef PONA_POSIX
	char* pathUtf8_;
#else
	wchar_t* pathWc_;
#endif
};

} // namespace pona

#endif // PONA_FILE_HPP
