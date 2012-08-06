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
#include "Variant.hpp"
#include "SyntaxDefinition.hpp"
#include "Array.hpp"
#include "WireObject.hpp"

namespace ftl
{

class WireException: public StdException
{
public:
	WireException(const String& error, int line, int pos);
	~WireException() throw();
	const char* what() const throw();
private:
	String message_;
	String error_;
	int line_;
	int pos_;
};

class Wire;

class Wire: public SyntaxDefinition, public Singleton<Wire>
{
public:
	Ref<WireObject, Owner> parse(Ref<ByteArray> text);

private:
	friend class Singleton<Wire>;

	Wire();
	String parseConcatenation(Ref<ByteArray> text, Ref<Token> token);
	Ref<WireObject, Owner> parseObject(Ref<ByteArray> text, Ref<Token> token);
	Ref<WireList, Owner> parseArray(Ref<ByteArray> text, Ref<Token> token);
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
