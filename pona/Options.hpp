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
#include "UString.hpp"
#include "Variant.hpp"
#include "List.hpp"
#include "Syntax.hpp"

namespace pona
{

PONA_EXCEPTION(OptionsException, Exception);

class Options: public Syntax<UString::Media>::Definition
{
public:
	Options();
	
	void define(char shortName, UString longName, Ref<Variant> value, UString description = "");
	Ref<UStringList, Owner> read(int argc, char** argv);
	
	UString entity(UString newValue = "");
	UString synopsis(UString newValue = "");
	UString summary(UString newValue = "");
	UString details(UString newValue = "");
	UString help();
	UString execPath() const;
	UString execName() const;
	UString execDir() const;
	
private:
	Ref<UStringList, Owner> read(UString line);
	
	RULE longNameRule_;
	RULE shortNameRule_;
	RULE valueRule_;
	RULE optionRule_;
	RULE options_;
	
	class Option: public Instance
	{
	public:
		char shortName_;
		UString longName_;
		UString description_;
		int typeMask_;
		// Variant defaultValue_;
		Ref<Variant, SetNull> value_;
	};
	
	typedef List< Ref<Option, Owner> > OptionList;
	Ref<OptionList, Owner> optionList_;
	UString entity_;
	UString synopsis_;
	UString summary_;
	UString details_;
	
	UString execPath_;
	UString execName_;
	UString execDir_;
	
	Ref<Option> optionByShortName(char name) const;
	Ref<Option> optionByLongName(UString name) const;
	void readOption(UString line, Ref<Token> token);
	void readValue(Ref<Option> option, UString line, Ref<Token> token);
	void verifyTypes();
};

} // namespace pona

#endif // PONA_OPTIONS_HPP
