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
#include <flux/stdio.h> // DEBUG
#include "Registry.h"
#include "CHeaderScanner.h"

namespace fluxclaim
{

CHeaderScannerInitializer::CHeaderScannerInitializer()
{
	if (count_++ == 0)
		Registry::instance()->registerHeaderScanner(new CHeaderScanner);
}

int CHeaderScannerInitializer::count_ = 0;

class CHeaderSyntax: public SyntaxDefinition
{
public:
	CHeaderSyntax();
};

CHeaderSyntax::CHeaderSyntax()
{
	DEFINE("Comment",
		GLUE(
			STRING("/*"),
			FIND(STRING("*/"))
		)
	);

	DEFINE("Header",
		GLUE(
			REPEAT(RANGE(" \t\n\r")),
			REF("Comment"),
			REPEAT(RANGE(" \t\n\r"))
		)
	);

	ENTRY("Header");

	LINK();
}

CHeaderScanner::CHeaderScanner()
	: headerSyntax_(new CHeaderSyntax)
{}

Ref<Header> CHeaderScanner::scanHeader(String path) const
{
	String text = File::open(path)->map();
	Ref<Token> rootToken = headerSyntax_->match(text, 0)->rootToken();
	if (!rootToken) return Ref<Header>();
	Token *token = rootToken->firstChild();
	String message = trimHeader(text->copy(token->i0() + 2, token->i1() - 2), " \t\r*");
	return Header::create(path, rootToken, text, message);
}

} // namespace fluxclaim
