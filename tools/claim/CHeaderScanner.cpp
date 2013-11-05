#include <flux/SyntaxDefinition.h>
#include <flux/File.h>
#include <flux/stdio.h> // DEBUG
#include "Registry.h"
#include "CHeaderScanner.h"

namespace fclaim
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
	Ref<Token> rootToken = headerSyntax_->match(text, 0);
	if (!rootToken) return Ref<Header>();
	Token *token = rootToken->firstChild();
	String message = trimHeader(text->copy(token->i0() + 2, token->i1() - 2), " \t\r*");
	return Header::create(path, rootToken, text, message);
}

} // namespace fclaim
