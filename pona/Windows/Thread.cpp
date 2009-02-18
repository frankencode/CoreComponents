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

#include "../Exception.hpp"
#include "../Thread.hpp"

namespace pona
{

Semaphore* Thread::tlsGlobalLock = new Semaphore(1);
Thread::Tls* Thread::tls = 0;

Thread::Thread()
	: handle_(0)
{}

Thread::~Thread()
{
	if (handle_)
	{
		CloseHandle(handle_);
		handle_ = 0;
	}
}

void Thread::start(int exitType)
{
	handle_ = CreateThread(0, 0, runWrapper, this, 0, 0);
	if (!handle_)
		PONA_WINDOWS_EXCEPTION;
	incRefCount(); // prevent self destruction while running
}

int Thread::wait()
{
	if (WaitForSingleObject(handle_, INFINITE) == WAIT_FAILED)
		PONA_WINDOWS_EXCEPTION;
	DWORD ret;
	if (!GetExitCodeThread(handle_, &ret))
		PONA_WINDOWS_EXCEPTION;
	return (int)ret;
}

void Thread::sleep(TimeStamp dt)
{
	Sleep(dt.miliSeconds());
}

DWORD Thread::runWrapper(LPVOID p)
{
	Thread* thread = (Thread*)p;
	Thread::tls = new Tls;
	int ret = thread->run();
	delete Thread::tls;
	thread->decRefCount(); // allow self destruction before termination
	return (DWORD)ret;
}

} // namespace pona
