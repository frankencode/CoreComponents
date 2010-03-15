/*
 * Options.hpp -- command line options
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_OPTIONS_HPP
#define PONA_OPTIONS_HPP

#include "atoms"
#include "String.hpp"
#include "Variant.hpp"
#include "NewList.hpp"
#include "Syntax.hpp"

namespace pona
{

PONA_EXCEPTION(OptionsException, Exception);

class Options: public Syntax<String::Media>::Definition
{
public:
	Options();
	
	void define(char shortName, String longName, Ref<Variant> value, String description = "");
	Ref<StringList, Owner> read(int argc, char** argv);
	
	String entity(String newValue = "");
	String synopsis(String newValue = "");
	String summary(String newValue = "");
	String details(String newValue = "");
	String help();
	String execPath() const;
	String execName() const;
	String execDir() const;
	
private:
	Ref<StringList, Owner> read(String line);
	
	RULE longNameRule_;
	RULE shortNameRule_;
	RULE valueRule_;
	RULE optionRule_;
	RULE options_;
	
	class Option: public Instance
	{
	public:
		char shortName_;
		String longName_;
		String description_;
		int typeMask_;
		// Variant defaultValue_;
		Ref<Variant, SetNull> value_;
	};
	
	typedef NewList< Ref<Option, Owner> > OptionList;
	Ref<OptionList, Owner> optionList_;
	String entity_;
	String synopsis_;
	String summary_;
	String details_;
	
	String execPath_;
	String execName_;
	String execDir_;
	
	Ref<Option> optionByShortName(char name) const;
	Ref<Option> optionByLongName(String name) const;
	void readOption(String line, Ref<Token> token);
	void readValue(Ref<Option> option, String line, Ref<Token> token);
	void verifyTypes();
};

} // namespace pona

#endif // PONA_OPTIONS_HPP
