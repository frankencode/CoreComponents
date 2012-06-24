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
#include "ByteArray.hpp"

namespace ftl
{

class SyntaxState: public Instance
{
public:
	SyntaxState()
		: hint_(0),
		  hintOffset_(-1)
	{}

	SyntaxState(int definitionId, int numFlags, int numChars, int numStrings, Ref<SyntaxState> parent = 0)
		: definitionId_(definitionId),
		  flags_(numFlags),
		  chars_(numChars),
		  strings_(numStrings),
		  hint_(0),
		  hintOffset_(-1)
	{
		if (parent)
			parent->child_ = this;
	}

	inline int definitionId() const { return definitionId_; }

	inline bool* flag(int id) { return flags_.pointerAt(id); }
	inline char* character(int id) { return chars_.pointerAt(id); }
	inline ByteArray* string(int id) { return strings_.pointerAt(id); }

	inline Ref<SyntaxState> child() const { return child_; }
	inline void setChild(Ref<SyntaxState> state) { child_ = state; }

	inline const char* hint() const { return hint_; }
	inline void setHint(const char* text) { hint_ = text; }

	inline int hintOffset() const { return hintOffset_; }
	inline void setHintOffset(int index) { hintOffset_ = index; }

private:
	int definitionId_;
	Array<bool> flags_;
	ByteArray chars_;
	Array<ByteArray> strings_;
	Ref<SyntaxState, Owner> child_;
	const char* hint_;
	int hintOffset_;
};

} // namespace ftl

#endif // FTL_SYNTAXSTATE_HPP
