/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Registry.h"
#include "PlaintextSyntax.h"

namespace flux {
namespace toki {

class PlaintextLanguage: public Language
{
private:
	friend class Registration<PlaintextLanguage>;

	PlaintextLanguage():
		Language(
			"Plaintext",
			Pattern("*.txt"),
			plaintextSyntax()
		)
	{}
};

namespace { Registration<PlaintextLanguage> registration; }

}} // namespace flux::toki
