 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_VARIANT_HPP
#define FTL_VARIANT_HPP

#include <new>
#include "atoms"
#include "String.hpp"

namespace ftl
{

// #ifdef __GNUC__
// #pragma pack(push,1)
// #endif

/** A variant can represent different types.
  * The type of a variant is defined implicitly at construction time or on assignment.
  * Variants automatically cast to bool, int, float if the
  * variant type is compatible with the target type requested by an expression.
  * In debug mode a DebugException will be thrown on illegal type casts.
  * You can check for type compatibility using compatibleTo() and for exact type match
  * by comparing type() with Variant::Type.
  * Variants can be savely passed by value. The size of a variant is ranges from 5
  * to 12 bytes depending on compiler and platform.
  * Variants provide 32 bit precision for both integer and floating point values.
  */
class Variant
{
public:
	enum Type {
		UndefType  = 0,
		IntType    = 1,
		BoolType   = 2 | IntType,
		FloatType  = 4,
		RefType    = 8,
		StringType = 16 | RefType,
		AnyType    = 31
	};

	Variant()                 : type_(UndefType)                {}
	Variant(int value)        : type_(IntType),   int_(value)   {}
	Variant(bool value)       : type_(BoolType),  int_(value)   {}
	Variant(float value)      : type_(FloatType), float_(value) {}
	Variant(double value)     : type_(FloatType), float_(value) {}
	Variant(const char *value): type_(StringType) { initRef(String(value)); }
	template<class T>
	Variant(O<T> value)       : type_(RefType)    { initRef(value); }
	Variant(String value)     : type_(StringType) { initRef(value); }
	~Variant() { if (type_ & RefType) killRef(); }

	inline const Variant &operator=(bool value)        { type_ = BoolType;  int_ = value; return *this; }
	inline const Variant &operator=(int value)         { type_ = IntType;   int_ = value; return *this; }
	inline const Variant &operator=(float value)       { type_ = FloatType; float_ = value; return *this; }
	inline const Variant &operator=(double value)      { type_ = FloatType; float_ = value; return *this; }
	inline const Variant &operator=(const char *value) { return *this = Variant(value); }
	inline const Variant &operator=(String value)      { return *this = Variant(value); }
	template<class T>
	inline const Variant &operator=(const O<T> &value) { return *this = Variant(value); }

	Variant(const Variant &b): type_(UndefType) { *this = b; }

	inline const Variant &operator=(const Variant &b) {
		if (type_ & RefType) killRef();
		type_ = b.type_;
		if (b.type_ & RefType)
			initRef(b.ref().get());
		else if (b.type_)
			int_ = b.int_;
		return *this;
	}

	inline int type() const { return type_; }
	inline bool defined() const { return type_ != UndefType; }
	inline bool compatibleTo(int type) { return type_ & type; }

	inline bool toBool() const {
		if (!type_) return false;
		FTL_ASSERT2(type_ & IntType, illegalConversion());
		return int_;
	}

	inline int toInt() const {
		if (!type_) return -1;
		FTL_ASSERT2(type_ & IntType, illegalConversion());
		return int_;
	}

	inline float toFloat() const {
		if (!type_) return ftl::nan;
		FTL_ASSERT2(type_ & FloatType, illegalConversion());
		return float_;
	}

	inline String toString() const {
		if (!type_) return String();
		FTL_ASSERT2(type_ & StringType, illegalConversion());
		return String(*this);
	}

	template<class T>
	inline T *toInstance() const {
		if (!type_) return null<T>();
		FTL_ASSERT2(type_ & RefType, illegalConversion());
		return cast<T>(ref().get());
	}

	inline operator bool() const { return toBool(); }
	inline operator int() const { return toInt(); }
	inline operator float() const { return toFloat(); }
	inline operator String() const { return toString(); }
	template<class T>
	inline operator O<T>() const { return toInstance<T>(); }

	bool operator==(const Variant &b) const
	{
		bool equal = false;

		if ((type_ & IntType) && (b.type_ & IntType))
			equal = (int_ == b.int_);
		else if ((type_ & FloatType) && (b.type_ & FloatType))
			equal = (float_ == b.float_);
		else if ((type_ == StringType) && (b.type_ == StringType))
			equal = (String(*this) == String(b));
		else if ((type_ == RefType) && (b.type_ == RefType))
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
		else if ((type_ == RefType) && (b.type_ == RefType))
			below = (ref().get() < b.ref().get());

		return below;
	}

	inline bool operator>(const Variant &b) const { return b < *this; }
	inline bool operator!=(const Variant &b) const { return !(*this == b); }
	inline bool operator<=(const Variant &b) const { return (*this < b) || (*this == b); }
	inline bool operator>=(const Variant &b) const { return (b < *this) || (*this == b); }

private:
	inline static const char *illegalConversion() { return "Illegal variant conversion"; }

	inline void initRef(Instance *instance = 0) {
		new(dummy_)O<Instance>(instance);
	}
	inline void killRef() {
		ref().~O<Instance>();
	}
	inline void setRef(Instance *instance) const {
		ref() = instance;
	}
	inline O<Instance> &ref() const {
		return *union_cast<O<Instance>*>(dummy_);
	}
	char type_;
	union {
		int32_t int_;
		float32_t float_;
		mutable char dummy_[sizeof(O<Instance>)];
	};
};

// #ifdef __GNUC__
// #pragma pack(pop)
// #endif

template<class U>
inline U *cast(const Variant &a) { return a.toInstance<U>(); }

typedef List<Variant> VariantList;

} // namespace ftl

#endif // FTL_VARIANT_HPP
