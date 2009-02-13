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

#ifndef PONA_THREAD_HPP
#define PONA_THREAD_HPP

#ifdef PONA_POSIX
#include <pthread.h>
#include "Atoms.hpp"
#else // PONA_WINDOWS
#include <Windows.h>
#include "Atoms.hpp"
#include "Semaphore.hpp"
#endif

namespace pona
{

#ifdef PONA_WINDOWS
class Condition;
#endif

class Thread: public Instance
{
public:
	enum ExitType { joinable, detached };

	Thread();
	~Thread();

	void start(int exitType = joinable);
	int wait();

	static void sleep(TimeStamp dt);

protected:
	virtual int run() = 0;

private:
#ifdef PONA_POSIX
	pthread_t tid_;
	static void* runWrapper(void* p);
#else // PONA_WINDOWS
	friend class Condition;
	HANDLE handle_;
	static DWORD WINAPI runWrapper(LPVOID p);

	class Tls
	{
	public:
		Tls(): wait(0), next(0) {}
		Semaphore wait;
		Tls* next;
	};

	__declspec(thread) static Tls* tls;

	static Semaphore* tlsGlobalLock;

#endif // platform switch
};

} // namespace pona

#endif // PONA_THREAD_HPP
