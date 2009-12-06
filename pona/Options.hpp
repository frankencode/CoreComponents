/*
 * Options.hpp -- command line options
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_OPTIONS_HPP
#define PONA_OPTIONS_HPP

#include "atoms"
#include "String.hpp"
#include "Variant.hpp"
#include "List.hpp"
#include "Syntax.hpp"

namespace pona
{

PONA_EXCEPTION(OptionsException, Exception);

class Options: public Syntax<String::Media>::Definition
{
public:
	Options();
	
	void define(Char shortName, String longName, Ref<Variant> value, String description = "");
	Ref<StringList> read(int argc, char** argv);
	
	String entity(String newValue = String());
	String synopsis(String newValue = String());
	String summary(String newValue = String());
	String details(String newValue = String());
	String help();
	String execPath() const;
	String execName() const;
	String execDir() const;
	
private:
	Ref<StringList> read(String line);
	
	RULE longNameRule_;
	RULE shortNameRule_;
	RULE valueRule_;
	RULE optionRule_;
	RULE options_;
	
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
	String entity_;
	String synopsis_;
	String summary_;
	String details_;
	
	String execPath_;
	String execName_;
	String execDir_;
	
	Ref<Option> optionByShortName(Char name) const;
	Ref<Option> optionByLongName(String name) const;
	void readOption(String line, Ref<Token> token);
	void readValue(Ref<Option> option, String line, Ref<Token> token);
	void verifyTypes();
};

} // namespace pona

#endif // PONA_OPTIONS_HPP
