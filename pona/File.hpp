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
	
	enum AccessFlags { Read = 4, Write = 2, Execute = 1 };
	
	enum CreateFlags {
		UserRead   = 0400,
		UserWrite  = 0200,
		UserExec   = 0100,
		GroupRead  = 0040,
		GroupWrite = 0020,
		GroupExec  = 0010,
		OtherRead  = 0004,
		OtherWrite = 0002,
		OtherExec  = 0001
	};

	enum SeekMethod {
		SeekBegin  = 1,
		SeekCurrent = 2,
		SeekEnd  = 3
	};

	enum StandardStreams
	{
		StandardInput = 0,
		StandardOutput = 1,
		StandardError = 2
	};

	File(String path);
	File(int standardStream);
	~File();

	String path() const;
	int openFlags() const;

	bool access(int flags) const;
	bool exists() const;
	void create(int mask = 0644);
	void unlink();

	void open(int flags = Read|Write);

	Offset seek(Offset distance, int method = SeekBegin);
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
