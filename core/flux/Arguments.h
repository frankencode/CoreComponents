/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_ARGUMENTS_H
#define FLUX_ARGUMENTS_H

#include "Variant.h"

namespace flux
{

class Arguments: public Object
{
public:
	static Ref<Arguments> parse(int argc, char **argv);
	void validate(VariantMap *prototype) const;
	void override(VariantMap *config) const;

	inline VariantMap *options() const { return options_; }
	inline StringList *items() const { return items_; }
	inline String execPath() const { return execPath_; }
	inline String toolName() const { return execPath_->fileName(); }

private:
	Arguments(int argc, char **argv);

	Ref<VariantMap> options_;
	Ref<StringList> items_;
	String execPath_;
};

} // namespace flux

#endif // FLUX_ARGUMENTS_H
