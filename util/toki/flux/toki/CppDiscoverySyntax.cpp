#include <flux/Singleton.h>
#include "CxxSyntax.h"
#include "CppDiscoverySyntax.h"

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
