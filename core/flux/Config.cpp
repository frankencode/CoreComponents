/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "File.h"
#include "Format.h"
#include "Pattern.h"
#include "Config.h"

namespace flux
{

Config::Config(String className)
	: YasonObject(className),
	  arguments_(StringList::create())
{}

Ref<Config> Config::read(String path, ConfigProtocol *protocol)
{
	Ref<Config> config;
	try {
		config = Yason::parse(File::open(path)->map(), protocol);
		config->path_ = path;
	}
	catch (SystemException &) {
		throw ConfigException(Format("Failed to open configuration file %%") << path);
	}
	catch (YasonException &ex) {
		throw ConfigException(Format("Error in configuration file\n%%:%%") << path << ex.what());
	}
	if (protocol) config->prototype_ = protocol->value(config->className());
	return config;
}

Ref<Config> Config::read(int argc, char **argv, Config *config)
{
	Ref<Config> newConfig;
	if (!config)
		config = newConfig = new Config;

	Pattern flag("{1..2:-}(&name:[^-]{[^=]}){0..1:=(&value:{1..:[^=]})}");

	for (int i = 1; i < argc; ++i)
	{
		String s = argv[i];
		if (s->at(0) != '-') {
			config->arguments_->append(s);
			continue;
		}

		if (s == "-h" || s == "-help" || s == "--help" || s == "-?")
			throw HelpError();

		Ref<SyntaxState> state = flag->newState();
		if (!flag->match(s, state))
			throw ConfigException(Format("Illegal option syntax: \"%%\"") << s);

		String name = s->copy(state->capture("name"));
		String valueText = s->copy(state->capture("value"));
		Variant value = true;

		if (valueText != "") {
			try {
				value = Yason::parse(valueText);
			}
			catch (YasonException &ex) {
				valueText = "\"" + valueText + "\"";
				value = Yason::parse(valueText);
			}
		}

		if (config->prototype_) {
			Variant defaultValue;
			if (!config->prototype_->lookup(name, &defaultValue))
				throw ConfigException(Format("No such option: \"-%%\"") << name);
			if (type(value) != type(defaultValue)) {
				throw ConfigException(
					Format("Option \"-%%\" expects type %%") << name << Variant::typeName(type(defaultValue), itemType(defaultValue))
				);
			}
		}

		config->establish(name, value);
	}

	if (config->prototype_) {
		if (config->prototype_->size() != config->size()) {
			YasonObject *prototype = config->prototype_;
			for (int i = 0; i < prototype->size(); ++i) {
				String name = prototype->keyAt(i);
				if (config->size() <= i || config->keyAt(i) != name)
					config->insert(name, prototype->valueAt(i));
			}
		}
	}

	return config;
}

Ref<Config> Config::read(int argc, char **argv, YasonObject *prototype)
{
	Ref<Config> config = new Config;
	config->prototype_ = prototype;
	return read(argc, argv, config);
}

Ref<YasonObject> ConfigProtocol::produce(YasonObject *prototype)
{
	String className;
	if (prototype) className = prototype->className();
	return new Config(className);
}

} // namespace flux
