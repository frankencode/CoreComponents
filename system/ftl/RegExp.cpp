/*
 * RegExp.cpp -- regular expression
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "Singleton.hpp"
#include "RegExp.hpp"

namespace ftl
{

class RegExpCompiler: public Syntax<ByteArray>::Definition, public Singleton<RegExpCompiler>
{
private:
	friend class Singleton<RegExpCompiler>;
	friend class RegExp;
	RegExpCompiler();
	void compile(Ref<ByteArray> pattern, Ref<RegExp> regExp);
};

RegExpCompiler::RegExpCompiler()
{
	DEFINE("Any", CHAR('#'));
	DEFINE("Begin", CHAR('^'));
	DEFINE("End", CHAR('$'));

	DEFINE("Char",
		CHOICE(
			GLUE(
				CHAR('\\'),
				CHOICE(
					GLUE(
						CHAR('u'),
						REPEAT(4, 4,
							CHOICE(
								RANGE('0', '9'),
								RANGE('a', 'f'),
								RANGE('A', 'F')
							)
						)
					),
					ANY()
				)
			),
			OTHER("#{}*[]|^$()")
		)
	);

	DEFINE("String",
		REPEAT(1,
			INLINE("Char")
		)
	);

	DEFINE("Range",
		GLUE(
			CHAR('['),
			REPEAT(0, 1, CHAR('^'))
			REPEAT(1,
				CHOICE(
					GLUE(
						REF("Char"),
						CHAR('-'),
						REF("Char")
					),
					REF("Char")
				)
			),
			CHAR(']')
		)
	);

	DEFINE("MinCount", REPEAT(1, RANGE('0', '9')));
	DEFINE("MaxCount", REPEAT(1, RANGE('0', '9')));

	DEFINE("Repeat",
		GLUE(
			CHOICE(
				REF("Char"),
				REF("Any"),
				REF("Range"),
				REF("Group"),
			),
			CHAR('{'),
			REPEAT(0, 1, REF("MinCount")),
			REPEAT(0, 1,
				GLUE(
					CHAR(','),
					REPEAT(0, 1, REF("MaxCount"))
				)
			),
			CHAR('}')
		)
	);

	DEFINE("Group",
		GLUE(
			CHAR('('),
			REF("Choice"),
			CHAR(')')
		)
	);

	DEFINE("Sequence",
		REPEAT(
			CHOICE(
				REF("Repeat"),
				REF("String"),
				REF("Any"),
				REF("Range"),
				REF("Group"),
				REF("Choice")
			)
		)
	);

	DEFINE("Find",
		GLUE(
			CHAR('*'),
			REF("Choice")
		)
	);

	DEFINE("Choice",
		GLUE(
			REF("Sequence"),
			REPEAT(
				CHAR('|'),
				REF("Sequence")
			)
		)
	);

	ENTRY("Choice");
	LINK();
}

void RegExpCompiler::compile(Ref<ByteArray> pattern, Ref<RegExp> regExp)
{
}

RegExp::RegExp(const String& pattern)
{
	RegExpCompiler::instance()->compile(pattern, this);
}

} // namespace ftl
