/*
 * TokenFactory.hpp -- token allocation/initialization
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_TOKENFACTORY_HPP
#define PONA_TOKENFACTORY_HPP

#include <new>
#include "Token.hpp"

namespace pona
{

class TokenFactory: public Instance
{
public:
	TokenFactory()
		: buf_(0),
		  bufSize_(0)
	{}
	
	TokenFactory(void* buf, int bufSize)
		: buf_((uint8_t*)buf),
		  bufSize_(bufSize)
	{}
	
	inline Ref<Token, Owner> produce()
	{
		Ref<Token, Owner> token;
		if (last_)
			if (last_->refCount() == 1)
			{
				token = last_;
				token->setType(-1);
			}
		
		if (!token)
		{
			if (buf_)
			{
				if (bufSize_ >= int(sizeof(Token)))
				{
					token = new(buf_)Token;
					token->liberate();
					bufSize_ -= sizeof(Token);
					buf_ += sizeof(Token);
				}
				else
					*(uint8_t*)0 = 0;
			}
			else
				token = new Token;
		}
		
		last_ = token;
		if (!rootToken_) rootToken_ = token;
		
		return token;
	}
	
	inline Ref<Token> rootToken() const { return rootToken_; }

private:
	uint8_t* buf_;
	int bufSize_;
	Ref<Token, Owner> rootToken_;
	Ref<Token, Owner> last_;
};

} // namespace pona

#endif // PONA_TOKENFACTORY_HPP
