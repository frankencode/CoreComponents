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

#include "File.hpp"
#include "Format.hpp"
#include "Pattern.hpp"
#include "Config.hpp"

namespace ftl
{

Config::Config(const char *path)
{
	try {
		Ref<File, Owner> file = File::newInstance(path);
		file->open(File::Read);
		String text = file->readAll();
		object_ = wire()->parse(text);
	}
	catch (StreamException &) {
		throw ConfigException(Format("Can't open configuration file %%") << path);
	}
	catch (WireException &ex) {
		throw ConfigException(Format("Syntax error in configuration file\n%%%%") << path << ex.what());
	}
}

Ref<StringList, Owner> Config::init(int argc, char **argv)
{
	Ref<StringList, Owner> extra = StringList::newInstance();
	for (int i = 1; i < argc; ++i) {
		String s = argv[i];
		if (s->at(0) != '-') {
			extra->append(s);
			continue;
		}

		if (!Pattern("-{1,2}(?name:[^-][^=]{})(=(?value:[^=]{1,})){0,1}")->match(s))
			throw ConfigException(Format("Illegal option syntax: \"%%\"") << s);
		/*if (s->head(2) == "--") s = s->tail(s->length() - 2);
		else s = s->tail(s->length() - 1);
		if (s->contains('=')) {

		}

		}
		else {
			Ref<StringList, Owner> parts = s->split('=');
			if (parts->length() != 2) {
				throw ConfigException(Format("Illegal option syntax: \"%%\"") << s);
				break;
			}
			try {
				object_->establish(parts->at(0), parts->at(1));
			}
			catch (WireObjectException &) {
				throw ConfigException(Format("No such configuration parameter: \"%%\"") << parts->at(0));
			}
		}*/
	}
	return extra;
}

} // namespace ftl
