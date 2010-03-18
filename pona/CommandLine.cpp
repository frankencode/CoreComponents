/*
 * CommandLine.cpp -- options of a command line tool
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "strings.hpp"
#include "Format.hpp"
#include "File.hpp"
#include "Path.hpp"
#include "CommandLine.hpp"

namespace pona
{

CommandLine::CommandLine()
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

Ref<CommandOption> CommandLine::define(char shortName, String longName, Variant defaultValue, String description)
{
	Ref<CommandOption, Owner> option = new CommandOption;
	option->shortName_ = shortName;
	option->longName_ = longName;
	option->description_ = description;
	option->typeMask_ = defaultValue.type();
	option->defaultValue_ = defaultValue;
	option->value_ = defaultValue;
	optionList_->append(option);
	return option;
}

Ref<CommandOption> CommandLine::optionByShortName(char name) const
{
	Ref<CommandOption> option;
	for (OptionList::Index i = optionList_->first(); optionList_->def(i); ++i) {
		Ref<CommandOption> candidate = optionList_->at(i);
		if (candidate->shortName_ == name) {
			option = candidate;
			break;
		}
	}
	return option;
}

Ref<CommandOption> CommandLine::optionByLongName(String name) const
{
	Ref<CommandOption> option;
	for (OptionList::Index i = optionList_->first(); optionList_->def(i); ++i) {
		Ref<CommandOption> candidate = optionList_->at(i);
		if (candidate->longName_ == name) {
			option = candidate;
			break;
		}
	}
	return option;
}

Ref<StringList, Owner> CommandLine::read(int argc, char** argv)
{
	execPath_ = argv[0];
	execName_ = Path(execPath_).fileName();
	execDir_ = Path(execPath_).reduce().makeAbsolute();
	Ref<StringList, Owner> line = new StringList;
	for (int i = 1; i < argc; ++i) {
		line->append(String(argv[i]));
		if (i != argc - 1)
			line->append(String(" "));
	}
	return read(line);
}

String stripQuotes(String s)
{
	if (s->size() > 0) {
		if ( ((s->at(0) == '"') || (s->at(0) == '\'')) &&
			 (s->at(s->size() - 1) == s->at(0)) )
		{
			s = String(s, 1, s->size() - 2);
		}
	}
	return s;
}

Ref<StringList, Owner> CommandLine::read(String line)
{
	int i0 = 0, i1 = -1;
	Ref<Token, Owner> rootToken;
	if (!match(line, i0, &i1, &rootToken))
		PONA_THROW(CommandLineException, "Unrecognized option syntax");
	
	Ref<Token> token = rootToken->firstChild();
	
	while (token) {
		if (token->ruleId() != optionRule_->id())
			break;
		readOption(line, token);
		token = token->nextSibling();
	}
	
	verifyTypes();
	
	Ref<StringList, Owner> files = new StringList;
	while (token) {
		files->append(stripQuotes(String(line, token->index(), token->length())));
		token = token->nextSibling();
	}
	
	return files;
}

void CommandLine::verifyTypes()
{
	for (OptionList::Index i = optionList_->first(); optionList_->def(i); ++i)
	{
		Ref<CommandOption> option = optionList_->at(i);
		
		if ( ((option->value_.type() & option->typeMask_) == 0) &&
		     (option->typeMask_ != 0) )
		{
			PONA_THROW(CommandLineException, "Unrecognized option syntax");
		}
	}
}

void CommandLine::readOption(String line, Ref<Token> token)
{
	token = token->firstChild();
	
	if (token->ruleId() == shortNameRule_->id())
	{
		while (token)
		{
			char name = line->at(token->index());
			Ref<CommandOption> option = optionByShortName(name);
			if (!option)
				PONA_THROW(CommandLineException, pona::strdup(String(Format("Unsupported option: '-%%'") << name)->data()));
			
			token = token->nextSibling();
			if (token) {
				if (token->ruleId() == valueRule_->id()) {
					readValue(option, line, token);
					token = token->nextSibling();
				}
				else {
					option->value_ = true;
				}
			}
			else {
				option->value_ = true;
			}
		}
	}
	else if (token->ruleId() == longNameRule_->id())
	{
		String name(line, token->index(), token->length());
		Ref<CommandOption> option = optionByLongName(name);
		if (!option)
			PONA_THROW(CommandLineException, pona::strdup(String(Format("Unsupported option: '--%%'") << name)->data()));
		
		token = token->nextSibling();
		if (!token)
			option->value_ = true;
		else
			readValue(option, line, token);
	}
}

void CommandLine::readValue(Ref<CommandOption> option, String line, Ref<Token> token)
{
	String s = stripQuotes(String(line, token->index(), token->length()));
	Variant& value = option->value_;
	
	if (value.type() == Variant::StringType) {
		value = s;
	}
	else if (value.type() == Variant::BoolType) {
		bool on = (s == "1") || (s == "H") || (s == "h") || (s == "on") || (s == "On") || (s == "true") || (s == "True");
		bool off = (s == "0") || (s == "L") || (s == "l") || (s == "off") || (s == "Off") || (s == "false") || (s == "False");
		if (!(on && off))
			PONA_THROW(CommandLineException, "Unrecognized option syntax");
		
		value = on;
	}
	else if (value.type()== Variant::IntType) {
		bool ok = false;
		int x = s.toInt(&ok);
		
		if (!ok)
			PONA_THROW(CommandLineException, "Unrecognized option syntax");
		
		value = x;
	}
	else if (value.type() == Variant::FloatType) {
		bool ok = false;
		double x = s.toFloat(&ok);
		
		if (!ok)
			PONA_THROW(CommandLineException, "Unrecognized option syntax");
		
		value = x;
	}
}

String CommandLine::entity(String newValue)
{
	if (newValue != "") entity_ = newValue;
	return newValue;
}

String CommandLine::synopsis(String newValue)
{
	if (newValue != "") synopsis_ = newValue;
	return newValue;
}

String CommandLine::summary(String newValue)
{
	if (newValue != "") summary_ = newValue;
	return newValue;
}

String CommandLine::details(String newValue)
{
	if (newValue != "") details_ = newValue;
	return newValue;
}

String CommandLine::helpText() const
{
	String synopsis = synopsis_;
	if (synopsis == "")
		synopsis = Format() << execName_ << " [OPTION]... [" << entity_ << "]...";
	
	String options;
	{
		Ref<StringList, Owner> lines = new StringList;
		int maxLength = 0;
		
		for (OptionList::Index i = optionList_->first(); optionList_->def(i); ++i) {
			Ref<CommandOption> option = optionList_->get(i);
			Format format("  ");
			if (option->shortName_)
				format << '-' << option->shortName_;
			if (option->longName_ != "") {
				if (option->shortName_)
					format << ", ";
				format << "--" << option->longName_;
				if (option->typeMask_ != 0) {
					if ((option->typeMask_ & (Variant::IntType|Variant::FloatType)) != 0) {
						format << "=" << option->defaultValue_;
					}
					else if ((option->typeMask_ & Variant::StringType) != 0) {
						format << "='" << option->defaultValue_ << "'";
					}
				}
			}
			String line(format);
			if (line->size() > maxLength) maxLength = line->size();
			lines->append(line);
		}
		
		String indent(maxLength + 2, ' ');
		
		OptionList::Index i = optionList_->first();
		StringList::Index j = lines->first();
		for (;optionList_->def(i) && lines->def(j); ++i, ++j)
		{
			Ref<CommandOption> option = optionList_->at(i);
			Format line;
			line << lines->at(j);
			if (lines->at(j)->size() < indent->size())
				line << String(indent->size() - lines->at(j)->size(), ' ');
			line << option->description_;
			line << "\n";
			lines->set(j, line);
		}
		
		options = lines;
	}
	
	Format text;
	text << "Usage: " << synopsis << "\n";
	if (summary_->size() > 0)
		text << summary_ << "\n";
	text << "\n";
	text << "Options:\n";
	text << options;
	if (details_->size() > 0)
		text << details_;
	text << "\n";
	
	return text;
}

String CommandLine::execPath() const { return execPath_; }
String CommandLine::execName() const { return execName_; }
String CommandLine::execDir() const { return execDir_; }

} // namespace pona
