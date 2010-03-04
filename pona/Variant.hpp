/*
 * Variant.hpp -- metamorph data type
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_VARIANT_HPP
#define PONA_VARIANT_HPP

#include "atoms"
#include "UString.hpp"

namespace pona
{

PONA_EXCEPTION(VariantException, Exception);

/** A variable of type 'Variant' can take any value of any type.
  * A variant is passed by value like a 'UString'. If converting
  * back to a specific type, the specific type must match the
  * variant's type or an exception of type 'VariantException' will
  * be thrown.
  *
  * Example:
  *   Variant s = "abc", x = 3.4, y = 2;
  *   print("s = %%\n", UString(s)); // OK
  *   print("x = %%\n", int(x)); // throws exception
  *   print("y = %%\n", int(y)); // OK
  *   int z = int(x) + int(y); // OK
  *   if (x.type() == Variant::IntType) { // discriminate on type
  *     int i = x;
  *     ... do smth ...
  *   }
  */
class Variant: public Instance
{
public:
	enum Type { UndefType = 0, BoolType = 1, IntType = 2, FloatType = 4, StringType = 8, RefType = 16 };
	
	Variant(): type_(UndefType) {}
	Variant(bool value): type_(BoolType), float_(value) {}
	Variant(int value): type_(IntType), float_(value) {}
	Variant(double value): type_(FloatType), float_(value) {}
	Variant(const char* value): type_(StringType), ref_(UString(value).media()) {}
	Variant(Ref<UString::Media, Owner> value): type_(StringType), ref_(value) {}
	Variant(UString value): type_(StringType), ref_(value.media()) {}
	Variant(Ref<Instance> value): type_(RefType), ref_(value) {}
	
	inline const Variant& operator=(bool value) { type_ = BoolType; float_ = value;  return *this; }
	inline const Variant& operator=(int value) { type_ = IntType; float_ = value; return *this; }
	inline const Variant& operator=(double value) { type_ = FloatType; float_ = value; return *this; }
	inline const Variant& operator=(Ref<UString::Media, Owner> value) { type_ = StringType; ref_ = value; return *this; }
	inline const Variant& operator=(const char* value) { type_ = StringType; ref_ = UString(value).media(); return *this; }
	inline const Variant& operator=(UString value) { type_ = StringType; ref_ = value.media(); return *this; }
	inline const Variant& operator=(Ref<Instance> value) { type_ = RefType; ref_ = value; return *this; }
	
	Variant(const Variant& b)
		: type_(b.type_),
		  float_(b.float_),
		  ref_(b.ref_)
	{}
	
	inline const Variant& operator=(const Variant& b) {
		type_ = b.type_;
		float_ = b.float_;
		ref_ = b.ref_;
		return *this;
	}
	
	inline int type() const { return type_; }
	inline void require(int typeMask) { if ((type_ & typeMask) == 0) PONA_THROW(VariantException, ""); }
	inline bool defined() const { return type_ != UndefType; }
	
	inline bool toBool() const { if (type_ != BoolType) PONA_THROW(VariantException, ""); return float_ != 0; }
	inline int toInt() const { if (type_ != IntType) PONA_THROW(VariantException, ""); return int(float_); }
	inline double toFloat() const { if (type_ != FloatType) PONA_THROW(VariantException, ""); return float_; }
	inline UString toString() const { if (type_ != StringType) PONA_THROW(VariantException, ""); return UString(toInstance<UString::Media>()); }
	template<class T>
	inline Ref<T> toInstance() const { if ((type_ != StringType) && (type_ != RefType)) PONA_THROW(VariantException, ""); return Ref<T>(ref_); }
	
	inline operator bool() const { return toBool(); }
	inline operator int() const { return toInt(); }
	inline operator double() const { return toFloat(); }
	inline operator Ref<Instance>() const { if (type_ != RefType) PONA_THROW(VariantException, ""); return ref_; }
	
	bool operator==(const Variant& b) const
	{
		const int number = BoolType|IntType|FloatType;
		bool equal = false;
		
		if (((type_ & number) != 0) && ((b.type_ & number) != 0)) {
			equal = (float_ == b.float_);
		}
		else if ((type_ == StringType) && (b.type_ == StringType)) {
			Ref<UString::Media> ma(ref_);
			Ref<UString::Media> mb(b.ref_);
			equal = (*ma == *mb);
		}
		else if ((type_ == RefType) && (b.type_ == RefType)) {
			equal = (ref_.get() == b.ref_.get());
		}
		else {
			equal = ((type_ == UndefType) && (b.type_ == UndefType));
		}
		
		return equal;
	}
	
	bool operator<(const Variant& b) const
	{
		const int number = BoolType|IntType|FloatType;
		bool below = false;
		
		if (((type_ & number) != 0) && ((b.type_ & number) != 0)) {
			below = (float_ < b.float_);
		}
		else if ((type_ == StringType) && (b.type_ == StringType)) {
			Ref<UString::Media> ma = ref_;
			Ref<UString::Media> mb = b.ref_;
			below = (*ma < *mb);
		}
		else if ((type_ == RefType) && (b.type_ == RefType)) {
			below = (ref_.get() < b.ref_.get());
		}
		
		return below;
	}
	
	inline bool operator>(const Variant& b) const { return b < *this; }
	inline bool operator!=(const Variant& b) const { return !(*this == b); }
	inline bool operator<=(const Variant& b) const { return (*this < b) || (*this == b); }
	inline bool operator>=(const Variant& b) const { return (b < *this) || (*this == b); }
	
private:
	int type_;
	double float_;
	Ref<Instance, Owner> ref_;
};

} // namespace pona

#endif // PONA_VARIANT_HPP
