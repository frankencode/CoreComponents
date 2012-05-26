/*
 * Config.hpp -- application configuration
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_CONFIGLINE_HPP
#define FTL_CONFIGLINE_HPP

#include "Wire.hpp"

namespace ftl
{

class ConfigException: public StdException
{
public:
	ConfigException(const String& reason): reason_(reason) {}
	~ConfigException() throw() {}
	const char* what() const throw() { return reason_; }
private:
	String reason_;
};

class Config: public Instance
{
public:
	Config(const char* path);

	Ref<StringList, Owner> init(int argc, char** argv);

	inline Ref<WireObject> object() const { return object_; }

private:
	Ref<WireObject, Owner> object_;
};

} // namespace ftl

#endif // FTL_CONFIGLINE_HPP
