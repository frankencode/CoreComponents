#include "TokenScreen.hpp"
#include "Token.hpp"

namespace pona
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
		child->glow(screen);
		i = child->i1_;
		child = child->nextSibling();
	}
	
	if (i < i1_)
		if (!screen->project(this, i, i1_))
			return false;
	
	return true;
}

} // namespace pona
