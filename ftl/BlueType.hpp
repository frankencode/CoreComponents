/*
 * BlueType.hpp -- Blue type
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_BLUETYPE_HPP
#define FTL_BLUETYPE_HPP

#include "atoms"
#include "Variant.hpp"
#include "Default.hpp"
#include "Map.hpp"
#include "BlueIdentifier.hpp"

namespace ftl
{

class BlueType;

class BlueProperty: public Instance
{
public:
	BlueProperty(const char* name, Ref<BlueType> type, const Variant& defaultValue)
		: name_(name), id_(name), type_(type), defaultValue_(defaultValue)
	{}
	
	inline const char* name() const { return name_; }
	inline Ref<BlueType> type() const { return type_; }
	inline Variant defaultValue() const { return defaultValue_; }
	
private:
	BlueIdentifier id_;
	const char* name_;
	Ref<BlueType, Owner> type_;
	Variant defaultValue_;
};

class BlueType: public Instance
{
public:
	typedef Map<BlueIdentifier, Ref<BlueProperty, Owner> > Properties;
	
	BlueType()
		: type_(Variant::UndefType)
	{}
	
	BlueType(Variant::Type type)
		: type_(type)
	{}
	
	BlueType(const char* name, Ref<BlueType> parent = Default<BlueType>::instance())
		: id_(name),
		  type_(Variant::RefType),
		  properties_(new Properties),
		  parent_(parent)
	{}
	
	inline bool isDefined() const { return type_ != Variant::UndefType; }
	inline Variant::Type type() const { return type_; }
	
	inline bool isAtomic() const { return !properties_; }
	inline Ref<Properties> properties() const { return properties_; }
	
	inline void define(const char* name, Ref<BlueType> type, const Variant& defaultValue = Variant())
	{
		Ref<BlueProperty, Owner> property = new BlueProperty(name, type, defaultValue);
		properties_->insert(property->id_, property);
	}
	
	inline Ref<BlueType> parent() const { return parent_; }
	
private:
	BlueIdentifier id_;
	Variant::Type type_;
	Ref<Properties, Owner> properties_;
	Ref<BlueType, Owner> parent_;
};

} // namespace ftl

#endif // FTL_BLUETYPE_HPP
