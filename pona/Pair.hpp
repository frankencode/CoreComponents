/*
 * Pair.cpp -- key, value pair
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_PAIR_HPP
#define PONA_PAIR_HPP

namespace pona
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
	inline void setValue(const Value& value) { value_ = value; }
	
private:
	Key key_;
	Value value_;
};

template<class Key, class Value>
inline Pair<Key, Value> pair(const Key& key, const Value& value) { return Pair<Key, Value>(key, value); }

} // namespace pona

#endif // PONA_PAIR_HPP
