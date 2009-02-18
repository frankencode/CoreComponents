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

#include "LinePrinter.hpp"
#include "NumberFormatting.hpp"
#include "LineParser.hpp"

namespace pona
{

LineParser::LineParser(String line, int options)
	: line_(line),
	  pos_(0),
	  options_(options),
	  syntaxError_(false)
{}

#define LINEPARSER_SYNTAX_ERROR(reason) \
{ \
	if (!syntaxError_) \
	{ \
		syntaxError_ = true; \
		if (options_ & strict) \
		{ \
			reason_ = format(String(reason) + " at position %% in line \"%%\".") % pos_ % line_; \
			PONA_THROW(FormattingException, reason_.strdup() /* HACK, leaks memory */ ); \
		} \
	} \
}

void LineParser::autoSkip()
{
	if (options_ & autoSkipSpace)
		skipSpace();
	
	if (options_ & autoSkipSeparator)
	{
		skipSeparator();
		if (options_ & autoSkipSpace)
			skipSpace();
	}
}

bool LineParser::eol() const
{
	return (pos_ >= int(line_.length()));
}

bool LineParser::syntaxError() const
{
	return syntaxError_;
}

bool LineParser::matched() const
{
	return eol() && !syntaxError();
}

String LineParser::readWord()
{
	if (syntaxError_)
		return String();
	
	autoSkip();
	
	String ret;
	int endPos = pos_;
	while (endPos < line_.length()) {
		if (!isWord(line_.saveGet(endPos))) break;
		++endPos;
	}
	
	if (pos_ == endPos)
	{
		LINEPARSER_SYNTAX_ERROR("Expected word");
	}
	else
	{
		ret = line_.copy(pos_, endPos-pos_);
		pos_ = endPos;
		autoSkip();
	}
	
	return ret;
}

String LineParser::readIdentifier()
{
	if (syntaxError_)
		return String();
	
	autoSkip();
	
	String ret;
	int endPos = pos_;
	if ((!isAlpha(line_.saveGet(endPos))) && (line_.saveGet(endPos) != '_'))
	{
		LINEPARSER_SYNTAX_ERROR("Expected identifier");
	}
	else
	{
		++endPos;
		while (isWord(line_.saveGet(endPos)))
			++endPos;
		ret = line_.copy(pos_, endPos-pos_);
		pos_ = endPos;
		autoSkip();
	}
	
	return ret;
}

String LineParser::readString(char enclosing)
{
	if (syntaxError_)
		return String();
	
	autoSkip();

	String ret;
	int endPos = pos_;
	if (enclosing == -1)
	{
		if ((line_.saveGet(endPos) == '\'') || (line_.saveGet(endPos) == '"'))
			enclosing = line_.saveGet(endPos);
		else {
			LINEPARSER_SYNTAX_ERROR("Expected string");
			return String();
		}
	}
	
	++endPos;
	bool escaped = false;
	int len = 0;
	while (endPos < int(line_.length()))
	{
		if ((!escaped) && (line_.saveGet(endPos) == enclosing))
			break;
		escaped = (line_.saveGet(endPos) == '\\');
		len += (!escaped);
		++endPos;
	}
	
	if (endPos == int(line_.length()))
	{
		LINEPARSER_SYNTAX_ERROR("Open string");
	}
	else
	{
		ret = String(len, ' ');
		escaped = false;
		for (int i = pos_ + 1, k = 0; i < endPos; ++i)
		{
			char ch = line_.saveGet(i);
			if (escaped)
			{
				if (ch == 'n')
					ch = '\n';
				else if (ch == 't')
					ch = '\t';
				else if (ch == 'r')
					ch = '\r';
				else if (ch == 'f')
					ch = '\f';
			}
			ret.set(k, ch);
			escaped = (ch == '\\');
			k += (!escaped);
		}
		
		pos_ = endPos + 1;
	}
	
	return ret;
}

bool LineParser::readBool()
{
	if (syntaxError_)
		return false;
	
	autoSkip();
	
	int length = 0;
	bool value = parseBool(line_, pos_, &length);
	
	if (length == 0)
		LINEPARSER_SYNTAX_ERROR("Expeted bool");
	
	pos_ += length;
	return value;
}

int LineParser::readInt()
{
	if (syntaxError_)
		return 0;
	
	autoSkip();
	
	int length = 0;
	uint64_t value; int sign;
	parseInteger(&value, &sign, line_, pos_, &length);
	
	if (length == 0)
		LINEPARSER_SYNTAX_ERROR("Expected integer");
	
	/** HACK, missing handling of out-of-range conditions
	  */
	
	pos_ += length;
	return int(sign * value);
}

double LineParser::readDouble()
{
	if (syntaxError_)
		return nan;
	
	autoSkip();
	
	int length = 0;
	double value = parseFloatingPoint(line_, pos_, &length);
	
	if (length == 0)
		LINEPARSER_SYNTAX_ERROR("Expected floating point");
	
	pos_ += length;
	return value;
}

String LineParser::readPunctuator()
{
	if (syntaxError_)
		return String();
	
	autoSkip();
	
	String ret;
	const int singleCapa = 8;
	char single[singleCapa] = {',',';','(',')','[',']','{','}'};
	
	int endPos = pos_;
	if (!isPunct(line_.saveGet(endPos))) {
		LINEPARSER_SYNTAX_ERROR("Expected punctuator");
		return String();
	}
	
	int i = 0;
	while ((i < singleCapa) && (line_.saveGet(endPos) != single[i]))
		++i;
	
	++endPos;
	
	if (i == singleCapa)
	{
		while (isPunct(line_.saveGet(endPos)))
			++endPos;
	}
	
	ret = line_.copy(pos_, endPos - pos_);
	pos_ = endPos;
	
	autoSkip();
	
	return ret;
}

void LineParser::readEol()
{
	if (syntaxError_)
		return;
	
	if (!eol())
		LINEPARSER_SYNTAX_ERROR("Expected end-of-line");
}

void LineParser::skipSeparator()
{
	if (syntaxError_)
		return;
	
	while (pos_ < line_.length())
	{
		if (!isSeparator(line_.saveGet(pos_)))
			break;
		++pos_;
	}
}

void LineParser::skipSpace()
{
	if (syntaxError_)
		return;
		
	while (pos_ < line_.length())
	{
		if (!isSpace(line_.saveGet(pos_)))
			break;
		++pos_;
	}
}

LineParser& LineParser::arg(bool& value)
{
	value = readBool();
	return *this;
}

LineParser& LineParser::arg(int& value)
{
	value = readInt();
	return *this;
}

LineParser& LineParser::arg(double& value)
{
	value = readDouble();
	return *this;
}

LineParser& LineParser::arg(String& value)
{
	autoSkip();
	
	if (pos_ == line_.length()) {
		LINEPARSER_SYNTAX_ERROR("Expected string");
		return *this;
	}
	
	if ((line_.saveGet(pos_) == '\'') || (line_.saveGet(pos_) == '"'))
		value = readString();
	else if (isPunct(line_.saveGet(pos_)))
		value = readPunctuator();
	else
		value = readWord();
	
	return *this;
}

} // namespace pona
