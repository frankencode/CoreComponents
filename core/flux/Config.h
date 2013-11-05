/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_CONFIG_H
#define FLUX_CONFIG_H

#include "Yason.h"
#include "UserException.h"

namespace flux
{

class ConfigException: public UserException
{
public:
	ConfigException(String message)
		: UserException(message)
	{}
};

class Config: public YasonObject
{
public:
	static Ref<Config> read(String path, YasonProtocol *protocol = 0);
	static Ref<Config> read(int argc, char **argv, Config *config = 0);
	static Ref<Config> read(int argc, char **argv, YasonObject *prototype);

	StringList *arguments() const;
	String path() const;

private:
	Config(String path = "");

	Ref<YasonObject> prototype_;
	Ref<StringList> arguments_;
	String path_;
};

} // namespace flux

#endif // FLUX_CONFIG_H
