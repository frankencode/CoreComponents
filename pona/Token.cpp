#include "TokenScreen.hpp"
#include "Token.hpp"

namespace pona
{

void Token::glow(TokenScreen* screen)
{
	int i = i0_;
	
	Ref<Token> child = firstChild();
	while (child) {
		if (i < child->i0_) {
			screen->project(this, i, child->i0_);
			i = child->i0_;
		}
		child->glow(screen);
		i = child->i1_;
		child = child->nextSibling();
	}
	
	if (i < i1_)
		screen->project(this, i, i1_);
}

} // namespace pona
