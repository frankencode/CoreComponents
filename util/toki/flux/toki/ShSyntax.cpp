/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Singleton.h>
#include "ShSyntax.h"

namespace flux {
namespace toki {

class ShSyntax: public SyntaxDefinition
{
protected:
	friend class Singleton<ShSyntax>;
	ShSyntax();
};

ShSyntax::ShSyntax()
{
	SYNTAX("sh");

	DEFINE("Comment",
		GLUE(
			CHAR('#'),
			FIND(
				CHOICE(
					CHAR('\n'),
					EOI()
				)
			)
		)
	);

	DEFINE("Source",
		REPEAT(
			CHOICE(
				REF("Comment"),
				ANY()
			)
		)
	);

	ENTRY("Source");
	LINK();
}

const SyntaxDefinition *shSyntax() { return Singleton<ShSyntax>::instance(); }

}} // namespace flux::toki
