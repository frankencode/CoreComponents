/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_TOOLOPTIONS_HPP
#define PONA_TOOLOPTIONS_HPP

#include "atoms"
#include "String.hpp"
#include "Variant.hpp"
#include "List.hpp"
#include "BooleanLiteral.hpp"
#include "IntegerLiteral.hpp"
#include "FloatLiteral.hpp"
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
	Ref<StringList> read(String line);
	
	String help(String synopsis = "", String summary = "", String details = "");
	
private:
	Ref<BooleanLiteral, Owner> booleanRule_;
	Ref<IntegerLiteral, Owner> integerRule_;
	Ref<FloatLiteral, Owner> floatRule_;
	RULE stringRule_;
	RULE longNameRule_;
	RULE shortNameRule_;
	RULE optionRule_;
	
	class Option: public Instance
	{
	public:
		Char shortName_;
		String longName_;
		String description_;
		int typeMask_;
		Ref<Variant, SetNull> value_;
	};
	
	typedef List< Ref<Option, Owner> > OptionList;
	Ref<OptionList, Owner> optionList_;
	String toolName_;
	
	Ref<Option> optionByShortName(Char name) const;
	Ref<Option> optionByLongName(String name) const;
	void readOption(String line, Ref<Token> token);
	void verifyTypes();
};

} // namespace pona

#endif // PONA_TOOLOPTIONS_HPP
