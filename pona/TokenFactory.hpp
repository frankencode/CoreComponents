/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstractions Library.
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

#ifndef PONA_TOKENFACTORY_HPP
#define PONA_TOKENFACTORY_HPP

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
	
	TokenFactory(uint8_t* buf, int bufSize)
		: buf_(buf),
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

