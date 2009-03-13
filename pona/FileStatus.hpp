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

#ifndef PONA_FILESTATUS_HPP
#define PONA_FILESTATUS_HPP

#include "atoms"
#include "String.hpp"
#include "File.hpp"

namespace pona
{

class FileStatus: public Instance
{
public:
	FileStatus(String path): path_(path) { update(); }
	
	inline String path() const { return path_; }
	
	inline bool exists() const { return exists_; }
	inline int mode() const { return mode_; }
	inline File::Offset size() const { return size_; }
	inline String owner() const { return owner_; }
	inline String group() const { return group_; }
	
	void update();
	
private:
	String path_;
	bool exists_;
	int mode_;
	File::Offset size_;
	String owner_;
	String group_;
};

} // namespace pona

#endif // PONA_FILESTATUS_HPP

