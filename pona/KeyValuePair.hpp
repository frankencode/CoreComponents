/*
 * KeyValuePair.cpp -- key, value tuple (used by 'Map')
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
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

