/*
 * BlueObject.hpp -- Blue object
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_BLUEOBJECT_HPP
#define FTL_BLUEOBJECT_HPP

#include "BlueType.hpp"

namespace ftl
{

class BlueObject: public Instance
{
public:
	typedef Map<BlueIdentifier, Variant> Values;
	
	BlueObject(Ref<BlueType> type)
		: values_(new Values),
		  type_(type)
	{}
	
	Ref<Values> values() const { return values_; }
	Ref<BlueType> type() const { return type_; }
	
private:
	Ref<Values, Owner> values_;
	Ref<BlueType, Owner> type_;
};

} // namespace ftl

#endif // FTL_BLUEOBJECT_HPP
