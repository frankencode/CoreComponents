/*
 * SyntaxState.hpp -- state information of the syntax production engine
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_SYNTAXSTATE_HPP
#define PONA_SYNTAXSTATE_HPP

#include "atoms"
#include "Array.hpp"

namespace pona
{

template<class Char>
class SyntaxState: public Instance
{
public:
	SyntaxState()
		: languageId_(-1)
	{}
	
	SyntaxState(int languageId, int numFlags, int numChars, int numStrings, Ref<SyntaxState> parent = 0)
		: languageId_(languageId),
		  flags_(numFlags),
		  chars_(numChars),
		  strings_(numStrings)
	{
		if (parent)
			parent->child_ = this;
	}
	
	inline int languageId() const { return languageId_; }
	
	inline bool* flag(int id) { return flags_.at(id); }
	inline Char* character(int id) { return chars_.at(id); }
	inline Array<Char>* string(int id) { return strings_.at(id); }
	
	inline Ref<SyntaxState> child() const { return child_; }
	inline void setChild(Ref<SyntaxState> state) { child_ = state; }
	
private:
	int languageId_;
	Array<bool> flags_;
	Array<Char> chars_;
	Array< Array<Char> > strings_;
	Ref<SyntaxState, Owner> child_;
};

} // namespace pona

#endif // PONA_SYNTAXSTATE_HPP
