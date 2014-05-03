/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/SyntaxDefinition.h>
#include <flux/File.h>
#include "Registry.h"
#include "ShHeaderScanner.h"

namespace fluxclaim
{

ShHeaderScannerInitializer::ShHeaderScannerInitializer()
{
	if (count_++ == 0)
		Registry::instance()->registerHeaderScanner(new ShHeaderScanner);
}

int ShHeaderScannerInitializer::count_ = 0;

class ShHeaderSyntax: public SyntaxDefinition
{
public:
	ShHeaderSyntax();
};

ShHeaderSyntax::ShHeaderSyntax()
{
	DEFINE("Comment",
		REPEAT(1,
			GLUE(
				REPEAT(RANGE(" \t")),
				CHAR('#'),
				FIND(
					CHAR('\n')
				)
			)
		)
	);

	DEFINE("Header",
		GLUE(
			REPEAT(0, 1,
				GLUE(
					REPEAT(RANGE(" \t")),
					STRING("#!"),
					FIND(
						CHAR('\n')
					)
				)
			),
			REF("Comment"),
			REPEAT(RANGE(" \t\n\r"))
		)
	);

	ENTRY("Header");

	LINK();
}

ShHeaderScanner::ShHeaderScanner()
	: headerSyntax_(new ShHeaderSyntax)
{}

Ref<Header> ShHeaderScanner::scanHeader(String path) const
{
	String text = File::open(path)->map();
	Ref<Token> rootToken = headerSyntax_->match(text, 0)->rootToken();
	if (!rootToken) return Ref<Header>();
	Token *token = rootToken->firstChild();
	String message = trimHeader(text->copy(token->i0(), token->i1()), " \t\r#");
	return Header::create(path, rootToken, text, message);
}

} // namespace fluxclaim
