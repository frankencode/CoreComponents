/*
 * Config.cpp -- application configuration
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "Format.hpp"
#include "Config.hpp"

namespace ftl
{

Config::Config(const char* path)
{
	try {
		Ref<File, Owner> file = File::newInstance(path);
		file->open(File::Read);
		String text = file->readAll();
		object_ = wire()->parse(text);
	}
	catch (StreamException&) {
		throw ConfigException(Format("Can't open configuration file %%") << path);
	}
	catch (WireException& ex) {
		throw ConfigException(Format("Syntax error in configuration file\n%%%%") << path << ex.what());
	}
}

Ref<StringList, Owner> Config::init(int argc, char** argv)
{
	Ref<StringList, Owner> files = new StringList;
	for (int i = 1; i < argc; ++i) {
		String s = argv[i];
		if (!s->contains('=')) {
			files->append(s);
			continue;
		}
		Ref<StringList, Owner> parts = s->split('=');
		if (parts->length() != 2) {
			throw ConfigException(Format("Illegal option syntax: \"%%\"") << s);
			break;
		}
		try {
			object_->setMember(parts->at(0), parts->at(1));
		}
		catch (WireObjectException&) {
			throw ConfigException(Format("No such configuration parameter: \"%%\"") << parts->at(0));
		}
	}
	return files;
}

} // namespace ftl
