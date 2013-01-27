 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "File.hpp"
#include "Format.hpp"
#include "Pattern.hpp"
#include "Config.hpp"

namespace ftl
{

O<Config> Config::create() { return new Config; }

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
		throw ConfigException(Format("Syntax error in configuration file\n%%:%%") << path << ex.what());
	}
}

void Config::read(int argc, char **argv)
{
	arguments_ = StringList::create();
	options_ = StringList::create();

	Pattern flag("-{1,2}(?name:[^-][^=]{})(=(?value:[^=]{1,})){0,1}");

	for (int i = 1; i < argc; ++i)
	{
		String s = argv[i];
		if (s->at(0) != '-') {
			arguments_->append(s);
			continue;
		}

		O<SyntaxState> state = flag->newState();
		if (!flag->match(s, state))
			throw ConfigException(Format("Illegal option syntax: \"%%\"") << s);

		options_->append(s);

		String name = s->copy(state->capture("name"));
		String valueText = s->copy(state->capture("value"));
		Variant value = true;
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

String Config::path() const { return path_; }

} // namespace ftl
