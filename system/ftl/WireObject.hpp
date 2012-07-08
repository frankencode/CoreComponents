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

class WireObjectException: public StdException
{
public:
	WireObjectException(const String& reason): reason_(reason) {}
	~WireObjectException() throw() {}
	const char* what() const throw() { return reason_; }
private:
	String reason_;
};

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

	Variant member(const String& name) const;
	void setMember(const String& name, const Variant& newValue);

private:
	friend class Wire;
	String className_;
};

} // namespace ftl

#endif // FTL_WIREOBJECT_HPP
