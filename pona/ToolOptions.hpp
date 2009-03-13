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
#include "List.hpp"
#include "SyntaxDefinition.hpp"

namespace pona
{

PONA_EXCEPTION(ToolOptionsException, Exception);

class ToolOptions: public SyntaxDefinition<String>
{
public:
	enum ValueType { BooleanType, IntegerType, FloatType, StringType };
	
	ToolOptions();
	
	void define(Char shortName, String longName = "", int type = BooleanType, String defaultValue = "", String description = "");
	void read(int argc, char** argv);
	void read(String line);
	
private:
	RULE boolean_;
	RULE integer_;
	RULE float_;
	RULE string_;
	
	class Option: public Instance
	{
	public:
		int type_;
		Char shortName_;
		String longName_;
		String defaultValue_;
		String description_;
	};
	
	typedef List<Ref<Option, Owner>> OptionList;
	Ref<OptionList, Owner> optionList_;
	
	Ref<Option> optionByShortName(Char name) const;
	Ref<Option> optionByLongName(String name) const;
}

} // namespace pona

#endif // PONA_TOOLOPTIONS_HPP
