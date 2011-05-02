/*
 * Pair.cpp -- key, value pair
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_PAIR_HPP
#define FTL_PAIR_HPP

namespace ftl
{

template<class Key, class Value>
class Pair
{
public:
	Pair()
	{}
	
	Pair(const Key& key)
		: key_(key), value_(Value())
	{}
	
	Pair(const Key& key, const Value& value)
		: key_(key), value_(value)
	{}
	
	inline bool operator<(const Pair& b) const { return key_ <  b.key_; }
	inline bool operator>(const Pair& b) const { return key_ >  b.key_; }
	inline bool operator==(const Pair& b) const { return key_ ==  b.key_; }
	inline bool operator!=(const Pair& b) const { return key_ !=  b.key_; }
	inline bool operator<=(const Pair& b) const { return key_ <=  b.key_; }
	inline bool operator>=(const Pair& b) const { return key_ >=  b.key_; }
	
	inline const Key& key() const { return key_; }
	inline void setKey(const Key& key) { key_ = key; }
	
	inline const Value& value() const { return value_; }
	inline Value& value() { return value_; }
	inline void setValue(const Value& value) { value_ = value; }
	
public:
	Key key_;
	Value value_;
};

template<class Key, class Value>
inline Pair<Key, Value> pair(const Key& key, const Value& value) { return Pair<Key, Value>(key, value); }

} // namespace ftl

#endif // FTL_PAIR_HPP
