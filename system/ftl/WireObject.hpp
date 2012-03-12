/*
 * WireObject.hpp -- Wire object
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

#include "Default.hpp"

namespace ftl
{

class WireObject: public Instance
{
public:
	typedef Map<String, Ref<WireObject, Owner> > ObjectMap;
	typedef List< Ref<WireObject, Owner> > ValueList;

	WireObject(String name, Ref<ObjectMap	> prototype = 0, Ref<ByteArray> text = 0, int i0 = -1, int i1 = -1)
		: name_(name), prototype_(prototype),
		  text_(text ? text : prototype->text()),
		  i0_(i0 >= 0 ? i0 : prototype->i0()),
		  i1_(i1 >= i0_ ? i1 : prototype->i1()),
		  properties_(prototype->properties()->clone()),
		  list_(list->items()->clone())
	{}

	inline Ref<WireObject> prototype() const { return prototype_; }
	inline String name() const { return name_; }
	inline String value() const {
		return text_ ? text_->copy(i0, i1) : Default<ByteArray>::instance();
	}

	inline Ref<ByteArray> text() const { return text_; }
	inline int i0() const { return i0_; }
	inline int i1() const { return i1_; }

	inline bool isNull() const { return !text_; }
	inline bool isAtomic() const { return (text_) && (properties_->length() == 0); }
	inline bool isObject() const { return properties_->length() > 0; }
	inline bool isList() const { return items_->length() > 0; }

	inline Ref<Properties> properties() const { return properties_; }
	inline Ref<Items> items() const { return items_; }

private:
	Ref<WireObject, Owner> prototype_;
	String name_;
	Ref<ByteArray, Owner> text_;
	int i0_, int i1_;
	Ref<Properties, Owner> properties_;
	Ref<Items, Owner> items_;
};

} // namespace ftl

#endif // FTL_WIREOBJECT_HPP
