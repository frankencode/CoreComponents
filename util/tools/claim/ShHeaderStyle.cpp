/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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
		SYNTAX("sh-header");

		DEFINE("Magic",
			GLUE(
				REPEAT(RANGE(" \t")),
				STRING("#!"),
				FIND(
					CHAR('\n')
				)
			)
		);

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
				REPEAT(0, 1, INLINE("Magic")),
				REF("Header")
			)
		);

		ENTRY("Script");

		LINK();
	}
};

class ShMagicSyntax: public SyntaxDefinition
{
public:
	ShMagicSyntax()
	{
		SYNTAX("sh-magic");
		IMPORT(Singleton<ShHeaderSyntax>::instance());
		DEFINE("Magic", INLINE("sh-header::Magic"));
		ENTRY("Magic");
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
		format << " " << c->holder() << ".\n";
	}
	format <<
		" #\n"
		" # " << notice->statement()->replace("\n", "\n # ") << "\n";
	format <<
		" #\n"
		" ##\n"
		"\n";
	return format;
}

int ShHeaderStyle::magicCount(String text) const
{
	Ref<Token> token = Singleton<ShMagicSyntax>::instance()->match(text, 0)->rootToken();
	if (!token) return 0;
	return token->i1();
}

} // namespace fluxclaim
