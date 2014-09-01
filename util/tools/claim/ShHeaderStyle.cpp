/*
 * Copyright (C) 2013-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/SyntaxDefinition.h>
#include <flux/Singleton.h>
#include <flux/File.h>
#include <flux/Format.h>
#include "ShHeaderStyle.h"

namespace fluxclaim {

class ShHeaderSyntax: public SyntaxDefinition
{
public:
	ShHeaderSyntax()
	{
		DEFINE("Header",
			GLUE(
				REPEAT(1,
					GLUE(
						REPEAT(RANGE(" \t")),
						CHAR('#'),
						FIND(
							CHAR('\n')
						)
					)
				),
				REPEAT(RANGE(" \t\n\r"))
			)
		);

		DEFINE("Script",
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
				REF("Header")
			)
		);

		ENTRY("Script");

		LINK();
	}
};

ShHeaderStyle::ShHeaderStyle():
	HeaderStyle("sh")
{}

Ref<Header> ShHeaderStyle::scan(String path) const
{
	String text = File::open(path)->map();
	Ref<Token> rootToken = Singleton<ShHeaderSyntax>::instance()->match(text, 0)->rootToken();
	if (!rootToken) return Ref<Header>();
	Token *token = rootToken->firstChild();
	String message = trimHeader(text->copy(token->i0(), token->i1()), " \t\r#");
	return Header::create(path, token, text, message);
}

String ShHeaderStyle::str(Notice *notice) const
{
	Format format;
	format << "##\n";
	CopyrightList *copyrights = notice->copyrights();
	for (int i = 0; i < copyrights->count(); ++i) {
		Copyright *c = copyrights->at(i);
		format << " # Copyright (C) ";
		if (c->yearStart() == c->yearEnd()) format << c->yearStart();
		else format << c->yearStart() << "-" << c->yearEnd();
		format << " " << c->holder() << "\n";
	}
	format <<
		" #\n"
		" # " << notice->statement()->replace("\n", "\n # ") << "\n";
	format <<
		" ##\n"
		"\n";
	return format;
}

} // namespace fluxclaim
