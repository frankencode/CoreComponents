/*
 * KeyValuePair.cpp -- key, value tuple (used by 'Map')
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_KEYVALUEPAIR_HPP
#define PONA_KEYVALUEPAIR_HPP

#include "NonCopyable.hpp"

namespace pona
{

template<class Key, class Value>
class KeyValuePair
{
public:
	KeyValuePair()
	{}
	
	KeyValuePair(const Key& key)
		: key_(key), value_(Value())
	{}
	
	KeyValuePair(const Key& key, const Value& value)
		: key_(key), value_(value)
	{}
	
	inline bool operator<(const KeyValuePair& b) const { return key_ <  b.key_; }
	inline bool operator>(const KeyValuePair& b) const { return key_ >  b.key_; }
	inline bool operator==(const KeyValuePair& b) const { return key_ ==  b.key_; }
	inline bool operator!=(const KeyValuePair& b) const { return key_ !=  b.key_; }
	inline bool operator<=(const KeyValuePair& b) const { return key_ <=  b.key_; }
	inline bool operator>=(const KeyValuePair& b) const { return key_ >=  b.key_; }
	
	inline const Key& key() const { return key_; }
	inline void setKey(const Key& key) { key_ = key; }
	
	inline const Value& value() const { return value_; }
	inline void setValue(const Value& value) { value_ = value; }
	
private:
	Key key_;
	Value value_;
};

} // namespace pona

#endif // PONA_KEYVALUEPAIR_HPP
