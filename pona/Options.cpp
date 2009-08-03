/*
 * Options.cpp -- options of a command line tool
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "Format.hpp"
#include "File.hpp"
#include "Path.hpp"
#include "Options.hpp"

namespace pona
{

Options::Options()
	: optionList_(new OptionList),
	  entity_("FILE")
{
	STATE_CHAR("quote", '\"');
	
	DEFINE("whitespace", REPEAT(1, RANGE(" \t")));
	
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
				RANGE('A', 'Z'),
				RANGE('0', '9')
			)
		);
	
	DEFINE("assignment",
		GLUE(
			REPEAT(0, 1, INLINE("whitespace")),
			CHAR('='),
			REPEAT(0, 1, INLINE("whitespace")),
			REF("value")
		)
	);
	
	valueRule_ =
		DEFINE("value",
			CHOICE(
				GLUE(
					AHEAD(RANGE("'\"")),
					GETCHAR("quote"),
					REPEAT(
						GLUE(
							CHOICE(
								CHAR('\\'),
								NOT(VARCHAR("quote"))
							),
							ANY()
						)
					),
					VARCHAR("quote")
				),
				REPEAT(1,
					GLUE(
						NOT(INLINE("whitespace")),
						ANY()
					)
				)
			)
		);
	
	optionRule_ =
		DEFINE("option",
			GLUE(
				CHOICE(
					GLUE(
						CHAR('-'),
						REPEAT(1, REF("shortName")),
						REPEAT(0, 1, INLINE("assignment"))
					),
					GLUE(
						STRING("--"),
						REF("longName"),
						REPEAT(0, 1, INLINE("assignment"))
					),
					GLUE(
						REF("longName"),
						INLINE("assignment")
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
					REF("value")
				)
			),
			REPEAT(0, 1, INLINE("whitespace")),
			EOI()
		)
	);
}

void Options::define(Char shortName, String longName, Ref<Variant> value, String description)
{
	Ref<Option> option = new Option;
	option->shortName_ = shortName;
	option->longName_ = longName;
	// *(option->value_) = *value;
	option->value_ = value;
	option->typeMask_ = value->type();
	option->description_ = description;
	optionList_->append(option);
}

Ref<Options::Option> Options::optionByShortName(Char name) const
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

Ref<Options::Option> Options::optionByLongName(String name) const
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

Ref<StringList> Options::read(int argc, char** argv)
{
	execPath_ = argv[0];
	execName_ = Path(execPath_).fileName();
	execDir_ = Path(execPath_).stripComponent().makeAbsolute();
	String line;
	for (int i = 1; i < argc; ++i) {
		line->append(String(argv[i]));
		if (i != argc - 1)
			line->append(' ');
	}
	return read(line);
}

String stripQuotes(String s)
{
	if (s->length() > 0) {
		if ( ((s->get(0) == '"') || (s->get(0) == '\'')) &&
			 (s->get(s->length() - 1) == s->get(0)) )
		{
			s = s->copy(1, s->length() - 2);
		}
	}
	return s;
}

Ref<StringList> Options::read(String line)
{
	int i0 = 0, i1 = -1;
	Ref<Token, Owner> rootToken;
	if (!match(line, i0, &i1, &rootToken))
		PONA_THROW(OptionsException, "Unrecognized option syntax");
	
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
		files->append(stripQuotes(line->range(token->index(), token->length())));
		token = token->nextSibling();
	}
	
	return files;
}

void Options::verifyTypes()
{
	for (int i = 0; i < optionList_->length(); ++i)
	{
		Ref<Option> option = optionList_->get(i);
		
		if ( ((option->value_->type() & option->typeMask_) == 0) &&
		     (option->typeMask_ != 0) )
		{
			PONA_THROW(OptionsException, "Unrecognized option syntax");
		}
	}
}

void Options::readOption(String line, Ref<Token> token)
{
	token = token->firstChild();
	
	if (token->rule() == shortNameRule_->id())
	{
		while (token)
		{
			Char name = line->get(token->index());
			Ref<Option> option = optionByShortName(name);
			if (!option)
				PONA_THROW(OptionsException, String(Format("Unsupported option: '-%%'") << name).utf8());
			
			token = token->nextSibling();
			if (token) {
				if (token->rule() == valueRule_->id()) {
					readValue(option, line, token);
					token = token->nextSibling();
				}
				else {
					*option->value_ = true;
				}
			}
			else {
				*option->value_ = true;
			}
		}
	}
	else if (token->rule() == longNameRule_->id())
	{
		String name = line->range(token->index(), token->length());
		Ref<Option> option = optionByLongName(name);
		if (!option)
			PONA_THROW(OptionsException, String(Format("Unsupported option: '--%%'") << name).utf8());
		
		token = token->nextSibling();
		if (!token)
			*option->value_ = true;
		else
			readValue(option, line, token);
	}
}

void Options::readValue(Ref<Option> option, String line, Ref<Token> token)
{
	String s = stripQuotes(line->copy(token->index(), token->length()));
	Ref<Variant> value = option->value_;
	
	if (value->type() == Variant::StringType) {
		*value = s;
	}
	else if (value->type() == Variant::BoolType) {
		bool on = (s == "1") || (s == "H") || (s == "h") || (s == "on") || (s == "On") || (s == "true") || (s == "True");
		bool off = (s == "0") || (s == "L") || (s == "l") || (s == "off") || (s == "Off") || (s == "false") || (s == "False");
		if (!(on || off))
			PONA_THROW(OptionsException, "Unrecognized option syntax");
		
		*value = on;
	}
	else if (value->type()== Variant::IntType) {
		bool ok = false;
		int x = toInt(s, &ok);
		
		if (!ok)
			PONA_THROW(OptionsException, "Unrecognized option syntax");
		
		*value = x;
	}
	else if (value->type() == Variant::FloatType) {
		bool ok = false;
		double x = toFloat(s, &ok);
		
		if (!ok)
			PONA_THROW(OptionsException, "Unrecognized option syntax");
		
		*value = x;
	}
}

String Options::entity(String newValue)
{
	if (newValue != "") entity_ = newValue;
	return newValue;
}

String Options::synopsis(String newValue)
{
	if (newValue != "") synopsis_ = newValue;
	return newValue;
}

String Options::summary(String newValue)
{
	if (newValue != "") summary_ = newValue;
	return newValue;
}

String Options::details(String newValue)
{
	if (newValue != "") details_ = newValue;
	return newValue;
}

String Options::help()
{
	if (synopsis_->length() == 0)
		synopsis_ = execName_ + " [OPTION]... [" << entity_ << "]...";
	
	String options;
	{
		Ref<StringList> lines = new StringList;
		int maxLength = 0;
		
		for (int i = 0; i < optionList_->length(); ++i) {
			Ref<Option> option = optionList_->get(i);
			Format line("  ");
			if (option->longName_ != "") {
				line << "--" << option->longName_;
				if (option->typeMask_ != 0) {
					if ((option->typeMask_ & (Variant::IntType|Variant::FloatType)) != 0) {
						line << "=" << *(option->value_);
					}
					else if ((option->typeMask_ & Variant::StringType) != 0) {
						line << "='" << *(option->value_) << "'";
					}
				}
				if (option->shortName_)
					line << ", ";
			}
			if (option->shortName_)
				line << '-' << option->shortName_;
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
		
		options = lines * String("");
	}
	
	String text = Format("Usage: %%\n") << synopsis_;
	if (summary_->length() > 0) text << summary_ << '\n';
	text << '\n';
	text << "Options:\n";
	text << options;
	if (details_->length() > 0)
		text << details_;
	text << '\n';
	return text;
}

String Options::execPath() const { return execPath_; }
String Options::execName() const { return execName_; }
String Options::execDir() const { return execDir_; }

} // namespace pona
