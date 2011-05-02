/*
 * Variant.hpp -- metamorph data type
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_VARIANT_HPP
#define FTL_VARIANT_HPP

#include <new>
#include "atoms"
#include "String.hpp"
#include "Path.hpp"

namespace ftl
{

// #ifdef __GNUC__
// #pragma pack(push,1)
// #endif

/** A variant can represent many different types.
  * The type of a variant is defined implicitly at construction time or on assignment.
  * Variants automatically cast to bool, int, float, String and Ref<Instance> if the
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
		UndefType    = 0,
		IntType      = 1,
		BoolType     = 2 | IntType,
		FloatType    = 4,
		RefType      = 8,
		StringType   = 16 | RefType,
		PathType     = 32 | RefType | StringType
	};
	
	Variant()                    : type_(UndefType)                {}
	Variant(bool value)          : type_(BoolType),  int_(value)   {}
	Variant(int value)           : type_(IntType),   int_(value)   {}
	Variant(float value)         : type_(FloatType), float_(value) {}
	Variant(double value)        : type_(FloatType), float_(value) {}
	Variant(const char* value)   : type_(StringType) { initRef(String(value).media()); }
	Variant(Ref<Instance> value) : type_(RefType)    { initRef(value); }
	Variant(String value)        : type_(StringType) { initRef(value.media()); }
	Variant(Path value)          : type_(PathType)   { initRef(value.toString().media()); }
	~Variant()                                       { if (type_ & RefType) killRef(); }
	
	inline const Variant& operator=(bool value)          { type_ = BoolType;   int_ = value; return *this; }
	inline const Variant& operator=(int value)           { type_ = IntType;    int_ = value; return *this; }
	inline const Variant& operator=(float value)         { type_ = FloatType;  float_ = value; return *this; }
	inline const Variant& operator=(double value)        { type_ = FloatType;  float_ = value; return *this; }
	inline const Variant& operator=(const char* value)   { type_ = StringType; setRef(String(value).media()); return *this; }
	inline const Variant& operator=(String value)        { type_ = StringType; setRef(value.media()); return *this; }
	inline const Variant& operator=(Path value)          { type_ = PathType;   setRef(value.toString().media()); return *this; }
	inline const Variant& operator=(Ref<Instance> value) { type_ = RefType;    setRef(value); return *this; }
	
	Variant(const Variant& b): type_(UndefType) { *this = b; }
	
	inline const Variant& operator=(const Variant& b) {
		type_ = b.type_;
		if (b.type_ & RefType) {
			if (type_ != RefType)
				initRef(b.ref().get());
			else
				ref() = b.ref();
		}
		else {
			if (type_ == RefType)
				killRef();
			int_ = b.int_;
		}
		return *this;
	}
	
	inline int type() const { return type_; }
	inline bool defined() const { return type_ != UndefType; }
	inline bool compatibleTo(int type) { return type_ & type; }
	
	inline bool toBool() const       { check(type_ & IntType,    illegalConversion()); return int_; }
	inline int toInt() const         { check(type_ & IntType,    illegalConversion()); return int_; }
	inline float toFloat() const     { check(type_ & FloatType,  illegalConversion()); return float_; }
	inline String toString() const   { check(type_ & StringType, illegalConversion()); return String(*this); }
	inline Path toPath() const       { check(type_ & StringType, illegalConversion()); return Path(String(*this)); }
	template<class T>
	inline Ref<T> toInstance() const { check(type_ & RefType,    illegalConversion()); return ref(); }
	
	inline operator bool() const { return toBool(); }
	inline operator int() const { return toInt(); }
	inline operator float() const { return toFloat(); }
	inline operator String() const { return toString(); }
	inline operator Path() const { return toPath(); }
	inline operator Ref<Instance>() const { return toInstance<Instance>(); }
	
	bool operator==(const Variant& b) const
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
	
	bool operator<(const Variant& b) const
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
	
	inline bool operator>(const Variant& b) const { return b < *this; }
	inline bool operator!=(const Variant& b) const { return !(*this == b); }
	inline bool operator<=(const Variant& b) const { return (*this < b) || (*this == b); }
	inline bool operator>=(const Variant& b) const { return (b < *this) || (*this == b); }
	
private:
	inline static const char* illegalConversion() { return "Illegal variant conversion."; }
	
	inline void initRef(Instance* instance) {
		new(dummy_)Ref<Instance, Owner>(instance);
	}
	inline void killRef() {
		ref().~Ref<Instance,Owner>();
	}
	inline void setRef(Instance* instance) const {
		ref() = instance;
	}
	inline Ref<Instance, Owner>& ref() const {
		return *reinterpret_cast<Ref<Instance, Owner>*>(dummy_);
	}
	char type_;
	union {
		int32_t int_;
		float32_t float_;
		mutable char dummy_[sizeof(Ref<Instance, Owner>)];
	};
};

// #ifdef __GNUC__
// #pragma pack(pop)
// #endif

} // namespace ftl

#endif // FTL_VARIANT_HPP
