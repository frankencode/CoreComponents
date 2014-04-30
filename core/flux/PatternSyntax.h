/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_PATTERNSYNTAX_H
#define FLUX_PATTERNSYNTAX_H

#include "String.h"
#include "Singleton.h"
#include "SyntaxDefinition.h"
#include "TextError.h"

namespace flux
{

class PatternSyntax: public SyntaxDefinition, public Singleton<PatternSyntax>
{
protected:
	friend class Singleton<PatternSyntax>;
	friend class Pattern;

	PatternSyntax();

	void compile(ByteArray *text, SyntaxDefinition *definition);
	NODE compileChoice(ByteArray *text, Token *token, SyntaxDefinition *definition);
	NODE compileSequence(ByteArray *text, Token *token, SyntaxDefinition *definition);
	NODE compileAhead(ByteArray *text, Token *token, SyntaxDefinition *definition);
	NODE compileBehind(ByteArray *text, Token *token, SyntaxDefinition *definition);
	NODE compileCapture(ByteArray *text, Token *token, SyntaxDefinition *definition);
	NODE compileReference(ByteArray *text, Token *token, SyntaxDefinition *definition);
	char readChar(ByteArray *text, Token *token);
	String readString(ByteArray *text, Token *token);
	NODE compileRangeMinMax(ByteArray *text, Token *token, SyntaxDefinition *definition);
	NODE compileRangeExplicit(ByteArray *text, Token *token, SyntaxDefinition *definition);
	NODE compileRepeat(ByteArray *text, Token *token, SyntaxDefinition *definition);

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
	int ahead_;
	int behind_;
	int identifier_;
	int capture_;
	int replay_;
	int group_;
	int choice_;
	int pattern_;
};

inline PatternSyntax *patternExpression() { return PatternSyntax::instance(); }

} // namespace flux

#endif // FLUX_PATTERNSYNTAX_H
