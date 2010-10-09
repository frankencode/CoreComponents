#include "streams"
#include "TokenScreen.hpp"
#include "Token.hpp"

namespace ftl
{

bool Token::glow(Ref<TokenScreen> screen)
{
	int i = i0_;
	
	Ref<Token> child = firstChild();
	while (child) {
		if (i < child->i0_) {
			if (!screen->project(this, i, child->i0_))
				return false;
			i = child->i0_;
		}
		if (!child->glow(screen))
			return false;
		i = child->i1_;
		child = child->nextSibling();
	}
	
	if (i < i1_)
		if (!screen->project(this, i, i1_))
			return false;
	
	return true;
}

void Token::meld(Ref<Token> root0, Ref<Token> root1)
{
	if ((!root0) || (!root1)) return;
	
	if (!root1->firstChild()) return;
	
	Token result_;
	Ref<Token, Pointer> result = &result_;
	Ref<Token, Owner> token0 = root0->firstChild();
	Ref<Token, Owner> token1 = root1->firstChild();
	
	// debug("Token::meld(): [%%: %%], [%%: %%]\n", root0->ruleName_, root0->countChildren(), root1->ruleName_, root1->countChildren());
	
	while (true)
	{
		bool take0 = (token0);
		bool take1 = (token1);
		if (take0 && take1) {
			take0 = (token0->i0_ < token1->i0_);
			take1 = !take0;
		}
		
		if (take0) {
			// debug("p0 [%%, %%, %%]\n", token0->ruleName_, token0->i0_, token0->i1_);
			Ref<Token, Owner> next0 = token0->nextSibling();
			Ref<Token> previousResult = result->lastChild();
			token0->unlink();
			result->insertChild(token0, previousResult);
			if (previousResult)
				token0->burn(previousResult->i0_, previousResult->i1_);
			token0 = next0;
		}
		else if (take1) {
			// debug("p1 [%%, %%, %%]\n", token1->ruleName_, token1->i0_, token1->i1_);
			Ref<Token, Owner> next1 = token1->nextSibling();
			Ref<Token> previousResult = result->lastChild();
			token1->unlink();
			if (previousResult) {
				if ((previousResult->i0_ < token1->i0_) && (token1->i1_ < previousResult->i1_)) {
					// see book 28/76: case 5
					Token dummy;
					dummy.appendChild(token1);
					meld(previousResult, &dummy);
					token1 = next1;
					continue;
				}
			}
			result->insertChild(token1, previousResult);
			if (previousResult)
				previousResult->burn(token1->i0_, token1->i1_);
			token1 = next1;
		}
		else
			break;
	}
	
	root0->appendAllChildrenOf(result);
}

bool Token::burn(int b0, int b1)
{
	// debug("burn: [%%, %%, %%] / (%%, %%)\n", ruleName_, i0_, i1_, b0, b1);
	
	// see book 28/76
	
	if ((i1_ <= b0) || (b1 <= i0_)) // case 3, 4
	{
		return false;
	}
	else if ((b0 <= i0_) && (i1_ <= b1)) // case 2
	{
		unlink();
		return true;
	}
	else if ((i0_ < b0) && (i1_ <= b1)) // case 0
	{
		i1_ = b0;
	}
	else if ((b0 <= i0_) && (b1 < i1_)) // case 1
	{
		i0_ = b1;
	}
	
	Ref<Token> child = firstChild();
	while (child) {
		Ref<Token> next = child->nextSibling();
		child->burn(b0, b1);
		child = next;
	}
	
	return true;
}

} // namespace ftl
