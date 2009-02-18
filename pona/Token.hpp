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

#ifndef PONA_TOKEN_HPP
#define PONA_TOKEN_HPP

#include "Tree.hpp"

namespace pona
{

class Token: public Tree
{
public:
	Token(): ruleName_("unknown"), language_(-1), rule_(-1), type_(-1) {}
	
	inline void join(Ref<Token> parent, const char* ruleName, int language, int rule, int i0, int i1)
	{
		if (parent)
			parent->appendChild(this);
		ruleName_ = ruleName;
		language_ = language;
		rule_ = rule;
		if (type_ == -1)
			type_ = rule;
		i0_ = i0;
		i1_ = i1;
	}
	
	inline const char* ruleName() { return ruleName_; }
	inline int language() const { return language_; }
	inline int rule() const { return rule_; }
	inline int type() const { return type_; }
	inline void setType(int value) { type_ = value; }
	
	inline int i0() const { return i0_; }
	inline int i1() const { return i1_; }
	inline int index() const { return i0_; }
	inline int length() const { return i1_ - i0_; }
	
	template<class Media>
	void print(LineSink* output, Media* media, int depth = 0)
	{
		for (int i = 0; i < depth; ++i)
			output->write("  ");
		output->write(format("%%(%%, %%, %%) = [") % ruleName_ % type_ % i0_ % i1_);
		for (int i = i0_; i < i1_; ++i) {
			output->write(format("%%") % media->get(i));
			if (i-i0_ > 10) {
				output->write("...");
				break;
			}
		}
		output->write("]\n");
		Ref<Token> token = firstChild();
		while (token) {
			token->print(output, media, depth + 1);
			token = token->nextSibling();
		}
	}
	
private:
	const char* ruleName_;
	int language_;
	int rule_;
	int type_;
	int i0_;
	int i1_;
};

} // namespace pona

#endif // PONA_TOKEN_HPP

