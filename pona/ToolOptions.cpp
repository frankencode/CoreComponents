#include "LinePrinter.hpp"
#include "File.hpp"
#include "ToolOptions.hpp"

namespace pona
{

ToolOptions::ToolOptions()
	: optionList_(new OptionList)
{
	booleanRule_ = new BooleanLiteral;
	integerRule_ = new IntegerLiteral;
	floatRule_ = new FloatLiteral;
	
	DEFINE("whitespace", REPEAT(1, RANGE(" \t")));
	
	stringRule_ =
		DEFINE("string",
			GLUE(
				NOT(STRING("--")),
				REPEAT(1,
					GLUE(
						NOT(INLINE("whitespace")),
						ANY()
					)
				)
			)
		);
	
	longNameRule_ =
		DEFINE("longName",
			GLUE(
				CHOICE(
					RANGE('a', 'z'),
					RANGE('A', 'Z'),
					CHAR('_')
				),
				REPEAT(
					CHOICE(
						RANGE('a', 'z'),
						RANGE('A', 'Z'),
						RANGE('0', '9'),
						RANGE("_-")
					)
				)
			)
		);
	
	shortNameRule_ =
		DEFINE("shortName",
			CHOICE(
				RANGE('a', 'z'),
				RANGE('A', 'Z')
			)
		);
	
	optionRule_ =
		DEFINE("option",
			GLUE(
				CHOICE(
					GLUE(
						STRING("--"),
						REF("longName"),
						REPEAT(0, 1,
							GLUE(
								REPEAT(0, 1, INLINE("whitespace")),
								CHAR('='),
								REPEAT(0, 1, INLINE("whitespace")),
								CHOICE(
									booleanRule_,
									floatRule_,
									integerRule_,
									stringRule_
								)
							)
						)
					),
					GLUE(
						CHAR('-'),
						REPEAT(1, REF("shortName"))
					)
				),
				CHOICE(
					EOI(),
					INLINE("whitespace")
				),
				NOT(CHAR('='))
			)
		);
	
	DEFINE_SELF(
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
					REF("string")
				)
			),
			REPEAT(0, 1, INLINE("whitespace")),
			EOI()
		)
	);
}

void ToolOptions::define(Char shortName, String longName, Ref<Variant> value, String description)
{
	Ref<Option> option = new Option;
	option->shortName_ = shortName;
	option->longName_ = longName;
	option->value_ = value;
	option->typeMask_ = value->type();
	option->description_ = description;
	optionList_->append(option);
}

Ref<ToolOptions::Option> ToolOptions::optionByShortName(Char name) const
{
	Ref<Option> option;
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
	Ref<Option> option;
	for (int i = 0, n = optionList_->length(); i < n; ++i) {
		Ref<Option> candidate = optionList_->get(i);
		if (candidate->longName_ == name) {
			option = candidate;
			break;
		}
	}
	return option;
}

Ref<StringList> ToolOptions::read(int argc, char** argv)
{
	toolName_ = File(argv[0]).name();
	String line;
	for (int i = 1; i < argc; ++i) {
		line->append(String(argv[i]));
		if (i != argc - 1)
			line->append(' ');
	}
	return read(line);
}

Ref<StringList> ToolOptions::read(String line)
{
	int i0 = 0, i1 = -1;
	Ref<Token, Owner> rootToken;
	if (!match(line, i0, &i1, &rootToken))
		PONA_THROW(ToolOptionsException, "Unrecognized option syntax");
	
	Ref<Token> token = rootToken->firstChild();
	
	while (token) {
		if (token->rule() != optionRule_->id())
			break;
		readOption(line, token);
		token = token->nextSibling();
	}
	
	verifyTypes();
	
	Ref<StringList> files = new StringList;
	while (token) {
		files->append(line->range(token->index(), token->length()));
		token = token->nextSibling();
	}
	
	return files;
}

void ToolOptions::verifyTypes()
{
	for (int i = 0; i < optionList_->length(); ++i)
	{
		Ref<Option> option = optionList_->get(i);
		
		if ( ((option->value_->type() & option->typeMask_) == 0) &&
		     (option->typeMask_ != 0) )
		{
			PONA_THROW(ToolOptionsException, "Unrecognized option syntax");
		}
	}
}

void ToolOptions::readOption(String line, Ref<Token> token)
{
	token = token->firstChild();
	
	if (token->rule() == shortNameRule_->id())
	{
		while (token)
		{
			Char name = line->get(token->index());
			Ref<Option> option = optionByShortName(name);
			if (!option)
				PONA_THROW(ToolOptionsException, String(format("Unsupported option: '-%%'") % name).strdup());
			
			*option->value_ = true;
			token = token->nextSibling();
		}
	}
	else if (token->rule() == longNameRule_->id())
	{
		String name = line->range(token->index(), token->length());
		Ref<Option> option = optionByLongName(name);
		if (!option)
			PONA_THROW(ToolOptionsException, String(format("Unsupported option: '--%%'") % name).strdup());
		
		Ref<Variant> value = option->value_;
		token = token->nextSibling();
		if (!token) {
			*option->value_ = true;
		}
		else {
			if ( (token->rule() == stringRule_->id()) ||
			     (option->value_->type() == Variant::StringType) )
			{
				*value = line->copy(token->index(), token->length());
			}
			else if (token->rule() == booleanRule_->id())
			{
				*value = booleanRule_->read(token);
			}
			else if (token->rule() == integerRule_->id())
			{
				uint64_t x; int s;
				integerRule_->read(line, token, &x, &s);
				
				if (value->type() == Variant::FloatType)
				{
					*value = 1. * s * x;
				}
				else if ( (value->type() == Variant::BoolType) &&
				          ((s == 0) || (x == 0) || (x == 1)) )
				{
					*value = bool(x);
				}
				else
					*value = s * int(x);
			}
			else if (token->rule() == floatRule_->id())
			{
				double x = floatRule_->read(line, token);
				if (value->type() == Variant::IntType)
					*value = int(x);
				else
					*value = x;
			}
		}
	}
}

String ToolOptions::help(String synopsis, String summary, String details)
{
	if (synopsis->length() == 0)
		synopsis = toolName_ + " [OPTION]... [FILE]...";
	
	String options;
	{
		Ref<StringList> lines = new StringList;
		int maxLength = 0;
		
		for (int i = 0; i < optionList_->length(); ++i) {
			Ref<Option> option = optionList_->get(i);
			String line;
			if ((option->shortName_ != 0) && (option->longName_->length() > 0))
				line = format("  -%%, --%%") % option->shortName_ % option->longName_;
			else if (option->shortName_ != 0)
				line = format("  -%%") % option->shortName_;
			else if (option->longName_->length() > 0)
				line = format("  --%%") % option->longName_;
			if (line->length() > maxLength) maxLength = line->length();
			lines->append(line);
		}
		
		String indent = (maxLength + 2) * Char(' ');
		
		for (int i = 0; i < optionList_->length(); ++i) {
			Ref<Option> option = optionList_->get(i);
			String line = lines->get(i);
			if (line->length() < indent->length())
				line << Char(' ') * (indent->length() - line->length());
			String text = option->description_;
			// text->replace("\n", "\n" + indent); // would be nice
			line << text << "\n";
		}
		
		// options = lines.join(); // would be nice
		for (int i = 0; i < lines->length(); ++i)
			options << lines->get(i);
	}
	
	String text = format("Usage: %%\n") % synopsis;
	if (summary->length() > 0) text << summary << '\n';
	text << '\n';
	text << "Options:\n";
	text << options;
	if (details->length() > 0)
		text << details;
	text << '\n';
	return text;
}

} // namespace pona
