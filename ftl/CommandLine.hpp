/*
 * CommandLine.hpp -- command line interface
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_COMMANDLINE_HPP
#define FTL_COMMANDLINE_HPP

#include "atoms"
#include "String.hpp"
#include "Variant.hpp"
#include "List.hpp"
#include "Syntax.hpp"
#include "CommandOption.hpp"

namespace ftl
{

FTL_EXCEPTION(CommandLineException, Exception);

class CommandLine: public Syntax<String::Media>::Definition
{
public:
	CommandLine();
	
	Ref<CommandOption> define(char shortName, String longName, Variant defaultValue = false, String description = "");
	String entity(String newValue = "");
	String synopsis(String newValue = "");
	String summary(String newValue = "");
	String details(String newValue = "");
	
	Ref<StringList, Owner> read(int argc, char** argv);
	
	typedef List< Ref<CommandOption, Owner> > OptionList;
	Ref<OptionList> usedOptions() const;
	
	String helpText() const;
	String execPath() const;
	String execName() const;
	String execDir() const;
	
private:
	Ref<StringList, Owner> read(String line);
	Ref<CommandOption> optionByShortName(char name) const;
	Ref<CommandOption> optionByLongName(String name) const;
	
	void readOption(String line, Ref<Token> token);
	void readValue(Ref<CommandOption> option, String line, Ref<Token> token);
	void verifyTypes();
	
	int longNameRule_;
	int shortNameRule_;
	int valueRule_;
	int optionRule_;
	
	Ref<OptionList, Owner> definedOptions_;
	Ref<OptionList, Owner> usedOptions_;
	String entity_;
	String synopsis_;
	String summary_;
	String details_;
	
	String execPath_;
	String execName_;
	String execDir_;
	
	int position_;
};

} // namespace ftl

#endif // FTL_COMMANDLINE_HPP
