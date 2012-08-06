/*
 * WireObject.hpp -- the Wire object
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_WIREOBJECT_HPP
#define FTL_WIREOBJECT_HPP

#include "Variant.hpp"
#include "Map.hpp"

namespace ftl
{

FTL_STD_EXCEPTION(WireObjectException);

class WireObject: public Map<String, Variant>
{
	typedef Map<String, Variant> Super;

public:
	WireObject() {}

	explicit WireObject(const WireObject& b)
		: Super(b),
		  className_(b.className_)
	{}

	virtual Ref<Super, Owner> clone() const { return new WireObject(*this); }

	inline String className() const { return className_; }

	bool hasMember(String path) const;
	Variant member(String path) const;
	void setMember(String path, Variant value);

private:
	friend class Wire;

	Ref<WireObject> resolve(String path, String* name = 0, Variant* value = 0) const;
	String className_;
};

typedef VariantList WireList;

} // namespace ftl

#endif // FTL_WIREOBJECT_HPP
