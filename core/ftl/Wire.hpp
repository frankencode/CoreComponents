 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_WIRE_HPP
#define FTL_WIRE_HPP

#include "Singleton.hpp"
#include "Array.hpp"
#include "Map.hpp"
#include "Variant.hpp"
#include "SyntaxDefinition.hpp"

namespace ftl
{

class WireException: public StdException
{
public:
	WireException(const String &error, int line, int pos);
	~WireException() throw();
	const char *what() const throw();
private:
	String message_;
	String error_;
	int line_;
	int pos_;
};

class Wire;

class WireObject: public Map<String, Variant>
{
	typedef Map<String, Variant> Super;

public:
	virtual Ref<Super, Owner> clone() const { return new WireObject(*this); }

	inline String className() const { return className_; }

protected:
	friend class Wire;

	WireObject() {}

private:
	explicit WireObject(const WireObject &b)
		: Super(b),
		  className_(b.className_)
	{}

	String className_;
};

class Wire: public SyntaxDefinition, public Singleton<Wire>
{
public:
	Variant parse(Ref<ByteArray> text, Ref<WireObject> virgin = 0);

protected:
	friend class Singleton<Wire>;

	Wire();

	String parseConcatenation(Ref<ByteArray> text, Ref<Token> token);
	Ref<WireObject, Owner> parseObject(Ref<ByteArray> text, Ref<Token> token, Ref<WireObject> virgin = 0);
	Ref<VariantList, Owner> parseList(Ref<ByteArray> text, Ref<Token> token);
	Variant parseValue(Ref<ByteArray> text, Ref<Token> token);

	int string_;
	int concatenation_;
	int specialValue_;
	int name_;
	int className_;
	int list_;
	int object_;
	int message_;
	int true_;
	int false_;
	int null_;
};

inline Ref<Wire> wire() { return Wire::instance(); }

} // namespace ftl

#endif // FTL_WIRE_HPP
