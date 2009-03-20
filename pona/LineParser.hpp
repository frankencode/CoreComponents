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

#ifndef PONA_LINEPARSER_HPP
#define PONA_LINEPARSER_HPP

#include "atoms"

namespace pona
{

class LineParser
{
public:
	enum Options
	{
		AutoSkipSpace = 1,
		AutoSkipSeparator = 2,
		Strict = 4,
		DefaultOptions = AutoSkipSpace | AutoSkipSeparator
	};
	
	LineParser(String line, int options = DefaultOptions);
	
	bool eol() const;
	bool syntaxError() const;
	bool matched() const;
	String reason() const { return reason_; }
	
	String readWord();
	String readIdentifier();
	String readString(Char enclosing = 0);
	bool readBool();
	int readInt();
	double readDouble();
	String readPunctuator();
	void readEol();
	
	void skipSeparator();
	void skipSpace();
	
	LineParser& arg(bool& value);
	LineParser& arg(int& value);
	LineParser& arg(double& value);
	LineParser& arg(String& value);
	
	template<class T>
	inline LineParser& operator%(T& value) { return arg(value); }
	
	inline operator bool() const { return matched(); }
	
private:
	void init();
	void autoSkip();
	inline static bool isAlpha(char ch) { return (('a' <= ch) && (ch <= 'z')) || (('A' <= ch) && (ch <= 'Z')); }
	inline static bool isDigit(char ch) { return ('0' <= ch) && (ch <= '9'); }
	inline static bool isWord(char ch) { return isAlpha(ch) || isDigit(ch) || (ch == '_'); }
	inline static bool isSpace(char ch) { return (ch == ' ') || (ch == '\t'); }
	inline static bool isPunct(char ch) { return !(isSpace(ch) || isWord(ch) || (ch == 0)); }
	inline static bool isSeparator(char ch) { return (ch == ',') || (ch == ';'); }
	
	inline Char saveGet(int i) const { return (i < line_->length()) ? line_->get(i) : Char(0); }
	
	String line_;
	int pos_;
	int options_;
	bool syntaxError_;
	String reason_;
};

} // namespace pona

#endif // PONA_LINEPARSER_HPP
