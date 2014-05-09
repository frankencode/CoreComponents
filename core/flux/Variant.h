/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_VARIANT_H
#define FLUX_VARIANT_H

#include <new>
#include "String.h"

namespace flux
{

class Variant;

typedef List<Variant> VariantList;

/** A Variant can represent different types.
  * The type of a Variant is defined implicitly at construction or assignment.
  * Variants automatically cast to bool, int, float if the Variant type is compatible
  * with the target type requested by an expression.
  * In debug mode a DebugException will be thrown on illegal type casts.
  * Variants can be savely passed by value.
  */
class Variant
{
public:
	typedef float real;

	enum Type {
		UndefType  = 0,
		IntType    = 1,
		BoolType   = 2 | IntType,
		FloatType  = 4,
		ObjectType = 8,
		StringType = 16 | ObjectType,
		ListType   = 32 | ObjectType,
		AnyType    = 63
	};

	static const char *typeName(int type, int itemType = UndefType);

	Variant():             type_(UndefType), itemType_(UndefType)                {}
	Variant(int value):    type_(IntType),   itemType_(UndefType), int_(value)   {}
	Variant(bool value):   type_(BoolType),  itemType_(UndefType), int_(value)   {}
	Variant(float value):  type_(FloatType), itemType_(UndefType), float_(value) {}
	Variant(double value): type_(FloatType), itemType_(UndefType), float_(value) {}

	Variant(const char *value):    type_(StringType), itemType_(UndefType) { initRef(String(value)); }
	Variant(Ref<ByteArray> value): type_(StringType), itemType_(UndefType) { initRef(value); }
	Variant(String value):         type_(StringType), itemType_(UndefType) { initRef(value); }
	template<class T>
	Variant(const Ref<T> &value): type_(ObjectType), itemType_(UndefType) { initRef(value); }

	Variant(const Ref< List<int> > &value):   type_(ListType), itemType_(IntType)    { initRef(value); }
	Variant(const Ref< List<bool> > &value):  type_(ListType), itemType_(BoolType)   { initRef(value); }
	Variant(const Ref< List<float> > &value): type_(ListType), itemType_(FloatType)  { initRef(value); }
	Variant(const Ref<StringList> &value):    type_(ListType), itemType_(StringType) { initRef(value); }
	Variant(const Ref<VariantList> &value):   type_(ListType), itemType_(AnyType)    { initRef(value); }

	Variant(const Variant &b): type_(UndefType), itemType_(UndefType) { *this = b; }

	~Variant() { if (type_ & ObjectType) killRef(); }

	inline const Variant &operator=(bool value)          { type_ = BoolType; int_ = value; return *this; }
	inline const Variant &operator=(int value)           { type_ = IntType;  int_ = value; return *this; }
	inline const Variant &operator=(float value)         { type_ = FloatType; float_ = value; return *this; }
	inline const Variant &operator=(double value)        { type_ = FloatType; float_ = value; return *this; }
	inline const Variant &operator=(const char *value)   { return *this = Variant(value); }
	inline const Variant &operator=(const String &value) { return *this = Variant(value); }
	template<class T>
	inline const Variant &operator=(const Ref<T> &value) { return *this = Variant(value); }

	inline const Variant &operator=(const Variant &b) {
		if (type_ & ObjectType) killRef();
		type_ = b.type_;
		itemType_ = b.itemType_;
		if (b.type_ & ObjectType)
			initRef(b.ref().get());
		else if (b.type_)
			int_ = b.int_;
		return *this;
	}

	inline operator bool() const {
		if (!type_) return bool();
		FLUX_ASSERT2(type_ & IntType, illegalConversion());
		return int_;
	}
	inline operator int() const {
		if (!type_) return int();
		FLUX_ASSERT2(type_ & IntType, illegalConversion());
		return int_;
	}
	inline operator float() const {
		if (!type_) return float();
		FLUX_ASSERT2(type_ & FloatType, illegalConversion());
		return float_;
	}
	inline operator String() const {
		if (!type_) return String();
		FLUX_ASSERT2(type_ & StringType, illegalConversion());
		return Ref<ByteArray>(ref());
	}

	template<class T>
	inline operator Ref<T>() const {
		if (!type_) return Ref<T>();
		FLUX_ASSERT2(type_ & ObjectType, illegalConversion());
		return cast<T>(ref().get());
	}

	bool operator==(const Variant &b) const
	{
		bool equal = false;

		if ((type_ & IntType) && (b.type_ & IntType))
			equal = (int_ == b.int_);
		else if ((type_ & FloatType) && (b.type_ & FloatType))
			equal = (float_ == b.float_);
		else if ((type_ == StringType) && (b.type_ == StringType))
			equal = (String(*this) == String(b));
		else if ((type_ == ObjectType) && (b.type_ == ObjectType))
			equal = (ref().get() == b.ref().get());
		else
			equal = ((type_ == UndefType) && (b.type_ == UndefType));

		return equal;
	}

	bool operator<(const Variant &b) const
	{
		bool below = false;

		if ((type_ & IntType) && (b.type_ & IntType))
			below = (int_ < b.int_);
		else if ((type_ & FloatType) && (b.type_ & FloatType))
			below = (float_ < b.float_);
		else if ((type_ == StringType) && (b.type_ == StringType))
			below = (String(*this) < String(b));
		else if ((type_ == ObjectType) && (b.type_ == ObjectType))
			below = (ref().get() < b.ref().get());

		return below;
	}

	inline bool operator>(const Variant &b) const  { return b < *this; }
	inline bool operator!=(const Variant &b) const { return !(*this == b); }
	inline bool operator<=(const Variant &b) const { return (*this < b) || (*this == b); }
	inline bool operator>=(const Variant &b) const { return (b < *this) || (*this == b); }

private:
	friend int type(const Variant &value);
	friend int itemType(const Variant &value);

	template<class U>
	friend U *cast(const Variant &value);

	inline static const char *illegalConversion() { return "Illegal Variant conversion"; }

	inline void initRef(Object *object = 0) {
		new(dummy_)Ref<Object>(object);
	}
	inline void killRef() {
		ref().~Ref<Object>();
	}
	inline void setRef(Object *object) const {
		ref() = object;
	}
	inline Ref<Object> &ref() const {
		return *union_cast< Ref<Object> *>(dummy_);
	}
	char type_;
	char itemType_;
	union {
		int int_;
		float float_;
		mutable char dummy_[sizeof(Ref<Object>)];
	};
};

inline int type(const Variant &value) { return value.type_; }
inline int itemType(const Variant &value) { return value.itemType_; }

template<class U>
inline U *cast(const Variant &value) { return type(value) & Variant::ObjectType ? cast<U>(value.ref()) : null<U>(); }

} // namespace flux

#endif // FLUX_VARIANT_H
