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
#include "variant.hpp"
#include "SyntaxDefinition.hpp"

namespace ftl
{

class WireException: public StdException
{
public:
	WireException(const string &error, int line, int pos);
	~WireException() throw();
	const char *what() const throw();
private:
	string message_;
	string error_;
	int line_;
	int pos_;
};

class Wire;

class WireObject: public Map<string, variant>
{
	typedef Map<string, variant> Super;

public:
	virtual Ref<Super> clone() const { return new WireObject(*this); }

	inline string className() const { return className_; }

protected:
	friend class Wire;

	WireObject() {}

private:
	explicit WireObject(const WireObject &b)
		: Super(b),
		  className_(b.className_)
	{}

	string className_;
};

class Wire: public SyntaxDefinition, public Singleton<Wire>
{
public:
	variant parse(ByteArray *text, WireObject *virgin = 0);

protected:
	friend class Singleton<Wire>;

	Wire();

	string parseConcatenation(ByteArray *text, Token *token);
	Ref<WireObject> parseObject(ByteArray *text, Token *token, WireObject *virgin = 0);
	Ref<VariantList> parseList(ByteArray *text, Token *token);
	variant parseValue(ByteArray *text, Token *token);

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

inline Wire *wire() { return Wire::instance(); }

} // namespace ftl

#endif // FTL_WIRE_HPP
