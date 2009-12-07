/*
 * Variant.hpp -- metamorph data type
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_VARIANT_HPP
#define PONA_VARIANT_HPP

#include "atoms"
#include "String.hpp"

namespace pona
{

PONA_EXCEPTION(VariantException, Exception);

/** A variable of type 'Variant' can take any value of any type.
  * A variant is passed by value like a 'String'. If converting
  * back to a specific type, the specific type must match the
  * variant's type or an exception of type 'VariantException' will
  * be thrown.
  *
  * Example:
  *   Variant s = "abc", x = 3.4, y = 2;
  *   print("s = %%\n", String(s)); // OK
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
	Variant(const char* value): type_(StringType), ref_(String(value).get()) {}
	Variant(Ref<String::Media, Owner> value): type_(StringType), ref_(value) {}
	Variant(String value): type_(StringType), ref_(value.get()) {}
	Variant(Ref<Instance> value): type_(RefType), ref_(value) {}
	
	inline const Variant& operator=(bool value) { type_ = BoolType; float_ = value;  return *this; }
	inline const Variant& operator=(int value) { type_ = IntType; float_ = value; return *this; }
	inline const Variant& operator=(double value) { type_ = FloatType; float_ = value; return *this; }
	inline const Variant& operator=(Ref<String::Media, Owner> value) { type_ = StringType; ref_ = value; return *this; }
	inline const Variant& operator=(const char* value) { type_ = StringType; ref_ = String(value).get(); return *this; }
	inline const Variant& operator=(String value) { type_ = StringType; ref_ = value.get(); return *this; }
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
	
	inline operator bool() const { if (type_ != BoolType) PONA_THROW(VariantException, ""); return float_ != 0; }
	inline operator int() const { if (type_ != IntType) PONA_THROW(VariantException, ""); return int(float_); }
	inline operator double() const { if (type_ != FloatType) PONA_THROW(VariantException, ""); return float_; }
	// inline operator Ref<String::Media, Owner>() const { if (type_ != StringType) PONA_THROW(VariantException, ""); return ref_; }
	inline operator String() const { if (type_ != StringType) PONA_THROW(VariantException, ""); return Ref<String::Media, Owner>(ref_); }
	inline operator Ref<Instance>() const { if (type_ != RefType) PONA_THROW(VariantException, ""); return ref_; }
	
	bool operator==(const Variant& b) const
	{
		const int number = BoolType|IntType|FloatType;
		bool equal = false;
		
		if (((type_ & number) != 0) && ((b.type_ & number) != 0)) {
			equal = (float_ == b.float_);
		}
		else if ((type_ == StringType) && (b.type_ == StringType)) {
			Ref<String::Media> ma(ref_);
			Ref<String::Media> mb(b.ref_);
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
			Ref<String::Media> ma = ref_;
			Ref<String::Media> mb = b.ref_;
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
