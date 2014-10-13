/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_PATTERNSYNTAX_H
#define FLUX_PATTERNSYNTAX_H

#include "String.h"
#include "SyntaxDefinition.h"

namespace flux
{

template<class SubClass> class Singleton;

class PatternSyntax: public SyntaxDefinition
{
protected:
	friend class Singleton<PatternSyntax>;
	friend class Pattern;

	PatternSyntax();

	void compile(const ByteArray *text, SyntaxDefinition *definition) const;
	NODE compileChoice(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
	NODE compileSequence(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
	NODE compileAhead(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
	NODE compileBehind(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
	NODE compileCapture(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
	NODE compileReference(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
	char readChar(const ByteArray *text, Token *token) const;
	String readString(const ByteArray *text, Token *token) const;
	NODE compileRangeMinMax(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
	NODE compileRangeExplicit(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;
	NODE compileRepeat(const ByteArray *text, Token *token, SyntaxDefinition *definition) const;

	int gap_;
	int any_;
	int boi_;
	int eoi_;
	int char_;
	int string_;
	int rangeMinMax_;
	int rangeExplicit_;
	int minRepeat_;
	int maxRepeat_;
	int repeat_;
	int sequence_;
	int group_;
	int ahead_;
	int behind_;
	int identifier_;
	int capture_;
	int replay_;
	int choice_;
	int pattern_;
};

const PatternSyntax *patternSyntax();

} // namespace flux

#endif // FLUX_PATTERNSYNTAX_H
