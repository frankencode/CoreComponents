 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "File.hpp"
#include "format.hpp"
#include "pattern.hpp"
#include "Config.hpp"

namespace ftl
{

hook<Config> Config::create() { return new Config; }

void Config::read(string path)
{
	try {
		path_ = path;
		wire()->parse(File::open(path, File::Read)->readAll(), this);
	}
	catch (SystemException &) {
		throw ConfigException(format("Can't open configuration file %%") << path);
	}
	catch (WireException &ex) {
		throw ConfigException(format("Syntax error in configuration file\n%%:%%") << path << ex.what());
	}
}

void Config::read(int argc, char **argv)
{
	arguments_ = StringList::create();
	options_ = StringList::create();

	pattern flag("-{1,2}(?name:[^-][^=]{})(=(?value:[^=]{1,})){0,1}");

	for (int i = 1; i < argc; ++i)
	{
		string s = argv[i];
		if (s->at(0) != '-') {
			arguments_->append(s);
			continue;
		}

		hook<SyntaxState> state = flag->newState();
		if (!flag->match(s, state))
			throw ConfigException(format("Illegal option syntax: \"%%\"") << s);

		options_->append(s);

		string name = s->copy(state->capture("name"));
		string valueText = s->copy(state->capture("value"));
		variant value = true;
		if (valueText != "") {
			try {
				value = wire()->parse(valueText);
			}
			catch (WireException &ex) {
				valueText = "\"" + valueText + "\"";
				value = wire()->parse(valueText);
			}
		}

		establish(name, value);
	}
}

StringList *Config::options() const { return options_; }
StringList *Config::arguments() const { return arguments_; }

string Config::path() const { return path_; }

} // namespace ftl
