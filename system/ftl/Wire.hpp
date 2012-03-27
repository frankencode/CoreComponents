/*
 * Wire.hpp -- a Wire language parser
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
#include "Syntax.hpp"
#include "Variant.hpp"
#include "Array.hpp"
#include "Map.hpp"

namespace ftl
{

FTL_EXCEPTION(WireException, Exception);

typedef Array<Variant> WireArray;
typedef Map<String, Variant> WireObject;

class Wire: public Syntax<ByteArray>::Definition, public Singleton<Wire>
{
public:
	Wire();
	Variant parse(String text);

protected:
	friend class Singleton<Wire>;
	int name_;
	int type_;
	int atom_;
	int array_;
	int member_;
	int source_;
};

inline Ref<Wire> wire() { return Wire::instance(); }

} // namespace ftl

#endif // FTL_WIRE_HPP
