/*
 * SyntaxState.hpp -- state information of the syntax production engine
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_SYNTAXSTATE_HPP
#define FTL_SYNTAXSTATE_HPP

#include "atoms"
#include "Array.hpp"

namespace ftl
{

template<class Char>
class SyntaxState: public Instance
{
public:
	SyntaxState()
	{}

	SyntaxState(int definitionId, int numFlags, int numChars, int numStrings, Ref<SyntaxState> parent = 0)
		: definitionId_(definitionId),
		  flags_(numFlags),
		  chars_(numChars),
		  strings_(numStrings),
		  hint_("")
	{
		if (parent)
			parent->child_ = this;
	}

	inline int definitionId() const { return definitionId_; }

	inline bool* flag(int id) { return flags_.pointerAt(id); }
	inline Char* character(int id) { return chars_.pointerAt(id); }
	inline Array<Char>* string(int id) { return strings_.pointerAt(id); }

	inline Ref<SyntaxState> child() const { return child_; }
	inline void setChild(Ref<SyntaxState> state) { child_ = state; }

	inline const char* hint() const { return hint_; }
	inline void setHint(const char* text) { hint_ = text; }

private:
	int definitionId_;
	Array<bool> flags_;
	Array<Char> chars_;
	Array< Array<Char> > strings_;
	Ref<SyntaxState, Owner> child_;
	const char* hint_;
};

} // namespace ftl

#endif // FTL_SYNTAXSTATE_HPP
