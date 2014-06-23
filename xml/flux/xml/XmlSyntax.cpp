/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/Singleton.h>
#include "XmlSyntax.h"

namespace flux {
namespace xml {

XmlSyntax::XmlSyntax()
{
	SYNTAX("xml");

	DEFINE_VOID("Gap",
		REPEAT(1, RANGE(" \t\n\r"))
	);

	DEFINE_VOID("Space",
		REPEAT(RANGE(" \t\n\r"))
	);

	DEFINE("Name",
		GLUE(
			CHOICE(
				RANGE('a', 'z'),
				RANGE('A', 'Z'),
				RAMGE(":_"),
				GREATER(0x7F)
			),
			REPEAT(
				CHOICE(
					RANGE('a', 'z'),
					RANGE('A', 'Z'),
					RAMGE(":_-."),
					RANGE('0', '9'),
					GREATER(0x7F)
				)
			)
		)
	);

	DEFINE("EntityReference",
		GLUE(
			CHAR('&'),
			INLINE("Name"),
			CHAR(';')
		)
	);

	DEFINE("CharReference",
		GLUE(
			STRING("&#"),
			CHOICE(
				REPEAT(1, RANGE('0', '9')),
				GLUE(
					CHAR('x'),
					REPEAT(1,
						CHOICE(
							RANGE('0', '9'),
							RANGE('a', 'f'),
							RANGE('A', 'F')
						)
					)
				)
			)
			CHAR(';')
		)
	);

	DEFINE("Reference",
		CHOICE(
			INLINE("EntityReference"),
			INLINE("CharReference")
		)
	);

	DEFINE("AttributeValue",
		GLUE(
			CAPTURE("stringTerminator", RANGE("\"'")),
			REPEAT(
				CHOICE(
					EXCEPT("<&"),
					REF("Reference")
				)
			),
			REPLAY("stringTerminator")
		)
	);

	DEFINE("CharData",
		REPEAT(EXCEPT("<&"))
	);

	DEFINE_VOID("Comment",
		GLUE(
			STRING("<!--"),
			REPEAT(
				CHOICE(
					OTHER('-'),
					GLUE(
						CHAR('-'),
						OTHER('-')
					)
				)
			),
			STRING("-->")
		)
	);

	DEFINE("ProcessingInstruction",
		GLUE(
			STRING("<?"),
			REF("Name"),
			AHEAD(
				CHOICE(
					INLINE("Gap"),
					STRING("?>")
				)
			),
			FIND(STRING("?>"))
		)
	);

	DEFINE("CharDataSection",
		GLUE(
			STRING("<![CDATA["),
			FIND(STRING("]]>"))
		)
	);

	DEFINE("XmlVersion",
		GLUE(
			STRING("1."),
			REPEAT(1, RANGE('0', '9')),
		)
	);

	DEFINE("XmlEncoding",
		GLUE(
			CHOICE(
				STRING("UTF-8"),
				STRING("utf-8"),
				GLUE(
					CHOICE(
						STRING("ISO"),
						STRING("iso")
					),
					STRING("-8859-"),
					RANGE('1', '9')
				)
			)
		)
	);

	DEFINE("XmlStandalone",
		CHOICE(
			STRING("yes"),
			STRING("no")
		)
	);

	DEFINE("XmlDeclaration",
		GLUE(
			STRING("<?xml"),
			INLINE("Gap"),
			STRING("version"),
			INLINE("Space"),
			CHAR('='),
			INLINE("Space"),
			CAPTURE("stringTerminator", RANGE("\"'")),
			REF("XmlVersion"),
			REPLAY("stringTerminator"),
			REPEAT(0, 1,
				GLUE(
					INLINE("Gap"),
					STRING("encoding"),
					INLINE("Space"),
					CHAR('='),
					INLINE("Space"),
					CAPTURE("stringTerminator", RANGE("\"'")),
					REF("XmlEncoding"),
					REPLAY("stringTerminator")
				)
			),
			REPEAT(0, 1,
				GLUE(
					INLINE("Gap"),
					STRING("standalone"),
					INLINE("Space"),
					CHAR('='),
					INLINE("Space"),
					CAPTURE("stringTerminator", RANGE("\"'")),
					REF("XmlStandalone"),
					REPLAY("stringTerminator")
				)
			),
			INLINE("Space"),
			STRING("?>")
		)
	);

	DEFINE("Attribute",
		GLUE(
			REF("Name"),
			INLINE("Space"),
			CHAR('='),
			INLINE("Space"),
			REF("AttributeValue")
		)
	);

	DEFINE("Element",
		GLUE(
			CHAR('<'),
			CAPTURE("tagName", REF("Name")),
			REPEAT(
				GLUE(
					INLINE("Gap"),
					REF("Attribute"),
				)
			),
			INLINE("Space")
			CHOICE(
				STRING("/>"),
				GLUE(
					CHAR('>'),
					REF("Content"),
					STRING("</"),
					REPLAY("tagName"),
					INLINE("Space"),
					CHAR('>')
				)
			)
		)
	);

	DEFINE("Content",
		GLUE(
			REPEAT(0, 1, REF("CharData")),
			REPEAT(
				GLUE(
					CHOICE(
						INVOKE("Element"),
						REF("Reference"),
						REF("CharDataSection"),
						REF("ProcessingInstruction"),
						INLINE("Comment"),
						REPEAT(0, 1, REF("CharData"))
					)
				)
			)
		)
	);

	DEFINE_VOID("Misc",
		CHOICE(
			REF("Comment"),
			REF("ProcessingInstruction"),
			INLINE("Gap")
		)
	);

	DEFINE("SystemLiteral",
		GLUE(
			CAPTURE("stringTerminator", RANGE("\"'")),
			FIND(REPLAY("stringTerminator"))
		)
	);

	DEFINE("PublicIdentifierLiteral",
		GLUE(
			CAPTURE("stringTerminator", RANGE("\"'")),
			REPEAT(
				GLUE(
					NOT(REPLAY("stringTerminator")),
					CHOICE(
						RANGE('a', 'z'),
						RANGE('A', 'Z'),
						RANGE('0', '9'),
						RANGE("-'()+,./:=?;!*#@$_%"),
						RANGE(" \n\r")
					)
				)
			),
			REPLAY("stringTerminator")
		)
	);

	DEFINE("ExternalIdentifier",
		CHOICE(
			GLUE(
				STRING("SYSTEM"),
				INLINE("Gap"),
				REF("SystemLiteral")
			),
			GLUE(
				STRING("PUBLIC"),
				INLINE("Gap"),
				REF("PublicIdentifierLiteral"),
				INLINE("Gap"),
				REF("SystemLiteral")
			)
		)
	);

	DEFINE("DocumentTypeDeclaration",
		GLUE(
			STRING("<!DOCTYPE"),
			INLINE("Gap"),
			REF("Name"),
			REPEAT(0, 1,
				GLUE(
					INLINE("Gap"),
					REF("ExternalIdentifier")
				)
			),
			INLINE("Space"),
			REPEAT(0, 1,
				GLUE(
					CHAR('['),
					REPEAT(
						CHOICE(
							INLINE("SystemLiteral"),
							OTHER(']')
						)
					),
					CHAR(']'),
					INLINE("Space")
				)
			),
			CHAR('>')
		)
	);

	DEFINE("Document",
		GLUE(
			REPEAT(0, 1, REF("XmlDeclaration")),
			REPEAT(INLINE("Misc")),
			REPEAT(0, 1,
				GLUE(
					INLINE("Gap"),
					REF("DocumentTypeDeclaration"),
					REPEAT(INLINE("Misc"))
				)
			),
			REF("Element"),
			REPEAT(INLINE("Misc"))
		)
	);

	ENTRY("Document");
	LINK();
}

XmlSyntax *xmlSyntax() { return Singleton<XmlSyntax>::instance(); }

}} // namespace flux::xml
