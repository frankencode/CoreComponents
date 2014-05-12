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

#include "yason.h"
#include "exceptions.h"

namespace flux
{

class ConfigProtocol;

class Config: public YasonObject
{
public:
	static Ref<Config> read(String path, ConfigProtocol *protocol = 0);
	static Ref<Config> read(int argc, char **argv, Config *config = 0);
	static Ref<Config> read(int argc, char **argv, YasonObject *prototype);

	inline StringList *arguments() const { return arguments_; }
	inline String path() const { return path_; }

private:
	friend class ConfigProtocol;

	Config(String className = "");

	Ref<YasonObject> prototype_;
	Ref<StringList> arguments_;
	String path_;
};

class ConfigProtocol: public YasonProtocol
{
protected:
	virtual Ref<YasonObject> produce(YasonObject *prototype);
};

} // namespace flux

#endif // FLUX_CONFIG_H
