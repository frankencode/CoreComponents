/*
 * ToolOptions.hpp -- options of a command line tool
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_TOOLOPTIONS_HPP
#define PONA_TOOLOPTIONS_HPP

#include "atoms"
#include "String.hpp"
#include "Variant.hpp"
#include "List.hpp"
#include "SyntaxDefinition.hpp"

namespace pona
{

PONA_EXCEPTION(ToolOptionsException, Exception);

class ToolOptions: public SyntaxDefinition<String::Media>
{
public:
	ToolOptions();
	
	void define(Char shortName, String longName, Ref<Variant> value, String description = "");
	Ref<StringList> read(int argc, char** argv);
	
	String help(String synopsis = "", String summary = "", String details = "");
	String toolName() const;
	String toolDir() const;
	
private:
	Ref<StringList> read(String line);
	
	RULE longNameRule_;
	RULE shortNameRule_;
	RULE valueRule_;
	RULE optionRule_;
	
	class Option: public Instance
	{
	public:
		Char shortName_;
		String longName_;
		String description_;
		int typeMask_;
		// Variant defaultValue_;
		Ref<Variant, SetNull> value_;
	};
	
	typedef List< Ref<Option, Owner> > OptionList;
	Ref<OptionList, Owner> optionList_;
	String toolName_;
	String toolDir_;
	
	Ref<Option> optionByShortName(Char name) const;
	Ref<Option> optionByLongName(String name) const;
	void readOption(String line, Ref<Token> token);
	void verifyTypes();
};

} // namespace pona

#endif // PONA_TOOLOPTIONS_HPP
