/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Singleton.h>
#include "PlaintextSyntax.h"

namespace flux {
namespace toki {

class PlaintextSyntax: public SyntaxDefinition
{
protected:
	friend class Singleton<PlaintextSyntax>;
	PlaintextSyntax();
};

PlaintextSyntax::PlaintextSyntax()
{
	SYNTAX("plaintext");

	DEFINE("Plaintext",
		REPEAT(
			ANY()
		)
	);

	ENTRY("Plaintext");
	LINK();
}

const SyntaxDefinition *plaintextSyntax()
{
	return Singleton<PlaintextSyntax>::instance();
}

}} // namespace flux::toki
