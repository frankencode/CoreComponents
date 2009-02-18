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

#include <Winsock2.h>
#include "../IoSyntaxSugar.hpp" // DEBUG
#include "../Mutex.hpp"
#include "WsaAwareness.hpp"

namespace pona
{

Ref<WsaAwareness, RefSetNull> WsaAwareness::instance()
{
	static Ref<WsaAwareness> instance_ = 0;
	
	if (!instance_) {
		static Mutex mutex;
		mutex.acquire();
		if (!instance_)
			instance_ = new WsaAwareness;
		mutex.release();
	}
	return instance_;
}

WsaAwareness::WsaAwareness()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
}

WsaAwareness::~WsaAwareness()
{
	WSACleanup();
}

} // namespace pona
