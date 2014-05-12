/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "yason.h"
#include "Pattern.h"
#include "Arguments.h"

namespace flux
{

Ref<Arguments> Arguments::parse(int argc, char **argv)
{
	return new Arguments(argc, argv);
}

Arguments::Arguments(int argc, char **argv)
	: options_(StringList::create()),
	  items_(StringList::create())
{
	execPath_ = argv[0];
	for (int i = 1; i < argc; ++i)
	{
		String s = argv[i];
		if (s->at(0) != '-') {
			items_->append(s);
			continue;
		}

		if (s == "-h" || s == "-help" || s == "--help" || s == "-?")
			throw HelpError();

		options_->append(s);
	}
}

Ref<YasonObject> Arguments::read(YasonObject *prototype) const
{
	Ref<YasonObject> object = prototype->produce();

	Pattern flag("{1..2:-}(@name:[^-]{[^=]}){0..1:=(@value:{1..:#})}");

	for (int i = 0; i < options_->size(); ++i)
	{
		String s = options_->at(i);

		Ref<SyntaxState> state = flag->match(s);
		if (!state->valid())
			throw UsageError(Format("Illegal option syntax: \"%%\"") << s);

		String name = s->copy(state->capture("name"));
		String valueText = s->copy(state->capture("value"));
		Variant value = true;

		if (valueText != "") {
			try {
				value = yason::parse(valueText);
			}
			catch (TextError &) {
				valueText = "\"" + valueText + "\"";
				try {
					value = yason::parse(valueText);
				}
				catch (TextError &) {
					throw UsageError(Format("Illegal option syntax: \"%%\"") << s);
				}
			}
		}

		if (prototype) {
			Variant defaultValue;
			if (!prototype->lookup(name, &defaultValue))
				throw UsageError(Format("No such option: \"-%%\"") << name);
			if (type(value) != type(defaultValue)) {
				throw UsageError(
					Format("Option \"-%%\" expects type %%") << name << Variant::typeName(type(defaultValue), itemType(defaultValue))
				);
			}
		}

		object->establish(name, value);
	}

	object->autocomplete(prototype);
	object->realize(0, 0);

	return object;
}

} // namespace flux
