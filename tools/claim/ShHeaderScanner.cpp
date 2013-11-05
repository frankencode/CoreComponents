#include <flux/SyntaxDefinition.h>
#include <flux/File.h>
#include "Registry.h"
#include "ShHeaderScanner.h"

namespace fclaim
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
	Ref<Token> rootToken = headerSyntax_->match(text, 0);
	if (!rootToken) return Ref<Header>();
	Token *token = rootToken->firstChild();
	String message = trimHeader(text->copy(token->i0(), token->i1()), " \t\r#");
	return Header::create(path, rootToken, text, message);
}

} // namespace fclaim
