#include "BooleanLiteral.hpp"
#include "IntegerLiteral.hpp"
#include "FloatLiteral.hpp"
#include "ToolOptions.hpp"

namespace pona
{

ToolOptions::ToolOptions()
	: optionList_(new OptionList)
{
	DEFINE("whitespace", REPEAT(1, RANGE(" \t")));
	
	DEFINE("option",
		CHOICE(
			GLUE(
				STRING("--"),
				REF("longName"),
				REPEAT(0, 1,
					GLUE(
						REPEAT(0, 1, INLINE("whitespace")),
						CHAR('='),
						REPEAT(0, 1, INLINE("whitespace")),
						REF("value")
					)
				)
			),
			GLUE(
				CHAR('-'),
				REPEAT(1, ("shortName"))
			)
		)
	);
	
	DEFINE("longName",
		GLUE(
			CHOICE(
				RANGE('a', 'z'),
				RANGE('A', 'Z'),
				CHAR('_')
			),
			REPEAT(
				RANGE('a', 'z'),
				RANGE('A', 'Z'),
				RANGE('0', '9'),
				CHAR('_')
			)
		)
	);
	
	DEFINE("shortName", RANGE('a', 'z'));
	
	boolean_ = new BooleanLiteral;
	integer_ = new IntegerLiteral;
	float_ = new FloatLiteral;
	
	string_ = DEFINE("string",
		REPEAT(1,
			GLUE(
				NOT(INLINE("whitespace")),
				ANY()
			)
		)
	);
	
	DEFINE("value",
		CHOICE(
			boolean_,
			float_,
			integer_,
			string_
		)
	);
	
	DEFINE_SELF("line",
		GLUE(
			REPEAT(
				GLUE(
					REPEAT(0, 1, INLINE("whitespace")),
					REF("option")
				)
			),
			REPEAT(
				GLUE(
					REPEAT(0, 1, INLINE("whitespace")),
					REF("string"),
				)
			),
			REPEAT(0, 1, INLINE("whitespace"))
		)
	);
}

void ToolOptions::define(Char shortName, String longName, int type, String defaultValue, String description)
{
	Ref<Option> option = new Option;
	option->type_ = type;
	option->shortName_ = shortName;
	option->longName_ = longName;
	option->defaultValue_ = defaultValue;
	option->description_ = description;
	optionList_->append(option);
}

Ref<ToolOptions::Option> ToolOptions::optionByShortName(Char name) const
{
	Re<Option> option;
	for (int i = 0, n = optionList_->length(); i < n; ++i) {
		Ref<Option> candidate = optionList_->get(i);
		if (candidate->shortName_ == name) {
			option = candidate;
			break;
		}
	}
	return option;
}

Ref<ToolOptions::Option> ToolOptions::optionByLongName(String name) const
{
	Re<Option> option;
	for (int i = 0, n = optionList_->length(); i < n; ++i) {
		Ref<Option> candidate = optionList_->get(i);
		if (candidate->longName_ == name) {
			option = candidate;
			break;
		}
	}
	return option;
}

void ToolOptions::read(int argc, char** argv)
{
	String line;
	for (int i = 1; i < argc; ++i)
		line.append(argv[i]);
	read(line);
}

void ToolOptions::read(String line)
{
	int i0 = 0, i1 = -1;
	Ref<Token, Owner> rootToken;
	if (!match(&line, i0, &i1, &rootToken))
		PONA_THROW(ToolOptionsException, "Syntax error");
	Ref<Token> token = rootToken->firstChild();
	while (token) {
		
		token = token->nextSibling();
	}
}

} // namespace pona
