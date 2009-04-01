/*
 * Token.hpp -- token of a token tree
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_TOKEN_HPP
#define PONA_TOKEN_HPP

#include "Tree.hpp"

namespace pona
{

class Token: public Tree
{
public:
	Token(): ruleName_("unknown"), language_(-1), rule_(-1), type_(-1) {}
	
	inline void init(const char* ruleName, int language, int rule)
	{
		ruleName_ = ruleName;
		language_ = language;
		rule_ = rule;
		if (type_ == -1)
			type_ = rule;
	}
	
	inline void setRange(int i0, int i1)
	{
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
	
	/*template<class Media>
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
	}*/
	
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

