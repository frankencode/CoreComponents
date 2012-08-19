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

FTL_STD_EXCEPTION(ConfigException);

class Config: public Instance
{
public:
	static Ref<Config, Owner> newInstance(const char *path) {
		return new Config(path);
	}

	Ref<StringList, Owner> init(int argc, char **argv);

	inline Ref<WireObject> object() const { return object_; }

private:
	Config(const char *path);

	Ref<WireObject, Owner> object_;
};

} // namespace ftl

#endif // FTL_CONFIGLINE_HPP
