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

#ifndef PONA_KEYVALUEPAIR_HPP
#define PONA_KEYVALUEPAIR_HPP

namespace pona
{

template<class Key, class Value>
class KeyValuePair
{
public:
	KeyValuePair()
	{}
	
	KeyValuePair(Key key, Value value = Value())
		: key_(key), value_(value)
	{}
	
	inline bool operator<(const KeyValuePair& b) const { return key_ <  b.key_; }
	inline bool operator>(const KeyValuePair& b) const { return key_ >  b.key_; }
	inline bool operator==(const KeyValuePair& b) const { return key_ ==  b.key_; }
	inline bool operator!=(const KeyValuePair& b) const { return key_ !=  b.key_; }
	inline bool operator<=(const KeyValuePair& b) const { return key_ <=  b.key_; }
	inline bool operator>=(const KeyValuePair& b) const { return key_ >=  b.key_; }
	
	inline Key key() const { return key_; }
	inline Value value() const { return value_; }
	
private:
	Key key_;
	Value value_;
};

} // namespace pona

#endif // PONA_KEYVALUEPAIR_HPP

