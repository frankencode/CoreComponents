/*
 * Wire.hpp -- the Wire parser
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
	inline bool flag(const char *name) {
		bool h = false;
		lookup(name, &h);
		return h;
	}

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
	int array_;
	int object_;
	int message_;
	int true_;
	int false_;
	int null_;
};

inline Ref<Wire> wire() { return Wire::instance(); }

} // namespace ftl

#endif // FTL_WIRE_HPP
