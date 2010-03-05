/*
 * Options.cpp -- options of a command line tool
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "strings.hpp"
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
	
	options_ =
		DEFINE_VOID("options",
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
	
	ENTRY("options");
	LINK();
}

void Options::define(char shortName, UString longName, Ref<Variant> value, UString description)
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

Ref<Options::Option> Options::optionByShortName(char name) const
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

Ref<Options::Option> Options::optionByLongName(UString name) const
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

Ref<UStringList, Owner> Options::read(int argc, char** argv)
{
	execPath_ = argv[0];
	execName_ = Path(execPath_).fileName();
	execDir_ = Path(execPath_).reduce().makeAbsolute();
	Ref<UStringList, Owner> line = new UStringList;
	for (int i = 1; i < argc; ++i) {
		line->append(UString(argv[i]));
		if (i != argc - 1)
			line->append(UString(" "));
	}
	return read(line);
}

UString stripQuotes(UString s)
{
	if (s->size() > 0) {
		if ( ((s->at(0) == '"') || (s->at(0) == '\'')) &&
			 (s->at(s->size() - 1) == s->at(0)) )
		{
			s = UString(s, 1, s->size() - 2);
		}
	}
	return s;
}

Ref<UStringList, Owner> Options::read(UString line)
{
	int i0 = 0, i1 = -1;
	Ref<Token, Owner> rootToken;
	if (!match(line, i0, &i1, &rootToken))
		PONA_THROW(OptionsException, "Unrecognized option syntax");
	
	Ref<Token> token = rootToken->firstChild();
	
	while (token) {
		if (token->ruleId() != optionRule_->id())
			break;
		readOption(line, token);
		token = token->nextSibling();
	}
	
	verifyTypes();
	
	Ref<UStringList, Owner> files = new UStringList;
	while (token) {
		files->append(stripQuotes(UString(line, token->index(), token->length())));
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

void Options::readOption(UString line, Ref<Token> token)
{
	token = token->firstChild();
	
	if (token->ruleId() == shortNameRule_->id())
	{
		while (token)
		{
			char name = line->at(token->index());
			Ref<Option> option = optionByShortName(name);
			if (!option)
				PONA_THROW(OptionsException, pona::strdup(UString(Format("Unsupported option: '-%%'") << name)->data()));
			
			token = token->nextSibling();
			if (token) {
				if (token->ruleId() == valueRule_->id()) {
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
	else if (token->ruleId() == longNameRule_->id())
	{
		UString name(line, token->index(), token->length());
		Ref<Option> option = optionByLongName(name);
		if (!option)
			PONA_THROW(OptionsException, pona::strdup(UString(Format("Unsupported option: '--%%'") << name)->data()));
		
		token = token->nextSibling();
		if (!token)
			*option->value_ = true;
		else
			readValue(option, line, token);
	}
}

void Options::readValue(Ref<Option> option, UString line, Ref<Token> token)
{
	UString s = stripQuotes(UString(line, token->index(), token->length()));
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
		int x = s.toInt(&ok);
		
		if (!ok)
			PONA_THROW(OptionsException, "Unrecognized option syntax");
		
		*value = x;
	}
	else if (value->type() == Variant::FloatType) {
		bool ok = false;
		double x = s.toFloat(&ok);
		
		if (!ok)
			PONA_THROW(OptionsException, "Unrecognized option syntax");
		
		*value = x;
	}
}

UString Options::entity(UString newValue)
{
	if (newValue != "") entity_ = newValue;
	return newValue;
}

UString Options::synopsis(UString newValue)
{
	if (newValue != "") synopsis_ = newValue;
	return newValue;
}

UString Options::summary(UString newValue)
{
	if (newValue != "") summary_ = newValue;
	return newValue;
}

UString Options::details(UString newValue)
{
	if (newValue != "") details_ = newValue;
	return newValue;
}

UString Options::help()
{
	if (synopsis_ == "")
		synopsis_ = Format() << execName_ << " [OPTION]... [" << entity_ << "]...";
	
	UString options;
	{
		Ref<UStringList, Owner> lines = new UStringList;
		int maxLength = 0;
		
		for (int i = 0; i < optionList_->length(); ++i) {
			Ref<Option> option = optionList_->get(i);
			Format format("  ");
			if (option->longName_ != "") {
				format << "--" << option->longName_;
				if (option->typeMask_ != 0) {
					if ((option->typeMask_ & (Variant::IntType|Variant::FloatType)) != 0) {
						format << "=" << *(option->value_);
					}
					else if ((option->typeMask_ & Variant::StringType) != 0) {
						format << "='" << *(option->value_) << "'";
					}
				}
				if (option->shortName_)
					format << ", ";
			}
			if (option->shortName_)
				format << '-' << option->shortName_;
			UString line(format);
			if (line->size() > maxLength) maxLength = line->size();
			lines->append(line);
		}
		
		UString indent(maxLength + 2, ' ');
		
		for (int i = 0; i < optionList_->length(); ++i) {
			Ref<Option> option = optionList_->get(i);
			Ref<UStringList, Owner> line = new UStringList;
			line->append(lines->get(i));
			if (lines->get(i)->size() < indent->size())
				line->append(UString(indent->size() - lines->get(i)->size(), ' '));
			line->append(option->description_);
			line->append("\n");
			lines->set(i, line);
		}
		
		options = lines;
	}
	
	Ref<UStringList, Owner> text = new UStringList;
	text->append(Format("Usage: %%\n") << synopsis_);
	if (summary_->size() > 0) {
		text->append(summary_);
		text->append("\n");
	}
	text->append("\n");
	text->append("Options:\n");
	text->append(options);
	if (details_->size() > 0)
		text->append(details_);
	text->append("\n");
	return text;
}

UString Options::execPath() const { return execPath_; }
UString Options::execName() const { return execName_; }
UString Options::execDir() const { return execDir_; }

} // namespace pona
