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

Ref<Config, Owner> Config::newInstance() { return new Config; }

void Config::read(String path)
{
	try {
		path_ = path;
		wire()->parse(File::open(path, File::Read)->readAll(), this);
	}
	catch (SystemException &) {
		throw ConfigException(Format("Can't open configuration file %%") << path);
	}
	catch (WireException &ex) {
		throw ConfigException(Format("Syntax error in configuration file\n%%%%") << path << ex.what());
	}
}

void Config::read(int argc, char **argv)
{
	arguments_ = StringList::newInstance();
	options_ = StringList::newInstance();

	Pattern flag("-{1,2}(?name:[^-][^=]{})(=(?value:[^=]{1,})){0,1}");

	for (int i = 1; i < argc; ++i)
	{
		String s = argv[i];
		if (s->at(0) != '-') {
			arguments_->append(s);
			continue;
		}

		Ref<SyntaxState, Owner> state = flag->newState();
		if (!flag->match(s, state))
			throw ConfigException(Format("Illegal option syntax: \"%%\"") << s);

		options_->append(s);

		String name = s->copy(state->capture("name"));
		String valueText = s->copy(state->capture("value"));
		Variant value = true;
		if (valueText != "") value = wire()->parse(valueText);

		establish(name, value);
	}
}

Ref<StringList> Config::options() const { return options_; }
Ref<StringList> Config::arguments() const { return arguments_; }

String Config::path() const { return path_; }

} // namespace ftl
