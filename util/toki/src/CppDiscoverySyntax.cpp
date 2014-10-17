/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/Singleton>
#include <flux/toki/CxxSyntax>
#include <flux/toki/CppDiscoverySyntax>

namespace flux {
namespace toki {

class CppDiscoverySyntax: public SyntaxDefinition
{
private:
	friend class Singleton<CppDiscoverySyntax>;
	CppDiscoverySyntax();
};

CppDiscoverySyntax::CppDiscoverySyntax()
{
	SYNTAX("cpp_discovery");
	IMPORT(cxxSyntax(), "cxx");

	DEFINE("Header",
		GLUE(
			REPEAT(
				CHOICE(
					REPEAT(1, RANGE(" \n\t\r")),
					INLINE("cxx::CommentText"),
					INLINE("cxx::CommentLine")
				)
			),
			CHAR('#'),
			RANGE(" \t"),
			KEYWORD("ifndef include")
		)
	);

	ENTRY("Header");
	LINK();
}

const SyntaxDefinition *cppDiscoverySyntax() { return Singleton<CppDiscoverySyntax>::instance(); }

}} // namespace flux::toki
