/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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
