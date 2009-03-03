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

#include <Windows.h>

#include "ProcessToken.hpp"
#include "../Exception.hpp"
#include "../Process.hpp"

namespace pona
{

/**
  * LIMITATIONS FOR WINDOWS (NT, XP and newer versions):
  *   1) An execPath has to be passed and Process::run() will never be entered.
  *      In other words you can't create worker clones. This limitation originates
  *      form the lack of a fork(2) system call under Windows.
  *   2) The method isSuperUser() returns always true.
  *      Just type in your favourite search engine "ntdll exploit" or "Windows rootkit"
  *      to learn more about how to implement MyProcess::makeRoot().
  *   3) Windows has no concept of friendly process termination and therefore
  *      terminate() has the same effect than directly calling kill().
  *   4) There is no support for session leadership implemented.
  *      This means startDaemon() is not working and setting process type to
  *      sessionLeader has no effect.
  *   5) You cannot start a new process with an empty environment.
  *      (Windows exposes here fancy undocumented behavior.)
  *   6) TcpStreams are no SystemStreams under Windows, so you can't simply pass
  *      TCP streams to a child process. This is a limitation of the OS,
  *      which is known for years and will probably never be fixed.
  *   7) On Windows there is no support for pseudo-terminals.
  *      (Afaik Windows supports only screen visible "consoles".)
  */

Process::Process()
	: processType_(simpleChild),
	  ioPolicy_(0),
	  pid_(-1)
{}

void Process::start()
{
	HANDLE inputPipeRead = 0;
	HANDLE inputPipeWrite = 0;
	HANDLE outputPipeRead = 0;
	HANDLE outputPipeWrite = 0;
	HANDLE errorPipeRead = 0;
	HANDLE errorPipeWrite = 0;
	
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;
	
	if (ioPolicy_ & ForwardInput) {
		if (!CreatePipe(&inputPipeRead, &inputPipeWrite, &sa, 0))
			PONA_WINDOWS_EXCEPTION;
		SetHandleInformation(inputPipeWrite, HANDLE_FLAG_INHERIT, 0);
	}
	
	if (ioPolicy_ & ForwardOutput) {
		if (!CreatePipe(&outputPipeRead, &outputPipeWrite, &sa, 0))
			PONA_WINDOWS_EXCEPTION;
		SetHandleInformation(outputPipeRead, HANDLE_FLAG_INHERIT, 0);
	}
	
	if (ioPolicy_ & ForwardError) {
		if (!CreatePipe(&errorPipeRead, &errorPipeWrite, &sa, 0))
			PONA_WINDOWS_EXCEPTION;
		SetHandleInformation(errorPipeRead, HANDLE_FLAG_INHERIT, 0);
	}
	
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	ZeroMemory(&startupInfo, sizeof(startupInfo));
	ZeroMemory(&processInfo, sizeof(processInfo));
	
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.dwFlags = STARTF_USESTDHANDLES;

	startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	startupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);

	if (ioPolicy_ & ForwardInput)
		startupInfo.hStdInput = inputPipeRead;
	if (ioPolicy_ & ForwardOutput)
		startupInfo.hStdOutput = outputPipeWrite;
	if (ioPolicy_ & ForwardError)
		startupInfo.hStdError = errorPipeWrite;
	
	if (rawInput_)
		startupInfo.hStdInput = rawInput_->handle();
	if (rawOutput_)
		startupInfo.hStdOutput = rawOutput_->handle();
	if (rawError_)
		startupInfo.hStdError = rawError_->handle();
	
	DWORD creationFlags = 0;
	if (processType_ == groupLeader)
		creationFlags = CREATE_NEW_PROCESS_GROUP;
	
	wchar_t* currentDirectoryWc = 0;
	if (workingDirectory_ != String()) {
		currentDirectoryWc  = workingDirectory_.wcsdup();
		if (execPath_.find("\\") == -1)
			execPath_ = ProcessStatus::workingDirectory() + "\\" + execPath_;
	}
	
	wchar_t* execPathWc = execPath_.wcsdup();
	
	wchar_t* commandLineWc = 0;
	if (argList_)
	{
		String s = execPath_;
		for (int i = 0, n = argList_->length(); i < n; ++i) {
			s.append(" ");
			s.append(argList_->get(i));
		}
		commandLineWc = s.wcsdup();
	}
	
	creationFlags |= CREATE_UNICODE_ENVIRONMENT;
	wchar_t* environmentWc = 0;
	if (envMap_)
	{
		Ref<List<EnvMap::Element>, Owner> envList = envMap_->toList();
		int size = 1;
		
		for (int i = 0, n = envList->length(); i < n; ++i)
			size += envList->get(i).key().length() + envList->get(i).value().length() + 2;

		environmentWc = (wchar_t*)malloc(sizeof(wchar_t) * size);
		int k = 0;

		for (int i = 0, n = envList->length(); i < n; ++i)
		{
			KeyValuePair<String, String> pair = envList->get(i);
			wchar_t* key = pair.key().wcsdup();
			wchar_t* value = pair.value().wcsdup();
			
			for (int j = 0; key[j]; ++j) {
				environmentWc[k] = key[j];
				++k;
			}
			environmentWc[k] = '=';
			++k;
			for (int j = 0; value[j]; ++j) {
				environmentWc[k] = value[j];
				++k;
			}
			environmentWc[k] = 0;
			++k;

			::free(key);
			::free(value);
		}
		environmentWc[k] = 0;
		++k;
	}
	
	BOOL ok = CreateProcess(
		/* lpApplicationName = */ execPathWc,
		/* lpCommandLine = */ commandLineWc,
		/* lpProcessAttributes = */ 0,
		/* lpThreadAttributes = */ 0,
		/* bInheritHandles = */ TRUE,
		/* dwCreationFlags = */ creationFlags,
		/* lpEnvironment = */ environmentWc,
		/* lpCurrentDirectory */ currentDirectoryWc,
		/* lpStartupInfo = */ &startupInfo,
		/* lpProcessInformation */ &processInfo
	);
	
	if (!ok)
		PONA_WINDOWS_EXCEPTION;
	
	::free(execPathWc);
	if (commandLineWc)
		::free(commandLineWc);
	if (environmentWc)
		::free(environmentWc);
	if (currentDirectoryWc)
		::free(currentDirectoryWc);
	
	handle_ = processInfo.hProcess;
	pid_ = processInfo.dwProcessId;
	if (!CloseHandle(processInfo.hThread))
		PONA_WINDOWS_EXCEPTION;

	if (ioPolicy_ & ForwardInput) {
		rawInput_ = new SystemStream(inputPipeWrite);
		input_ = new LineSink(rawInput_);
		CloseHandle(inputPipeRead);
	}
	else {
		rawInput_ = 0;
		input_ = 0;
	}
	
	if (ioPolicy_ & ForwardOutput) {
		rawOutput_ = new SystemStream(outputPipeRead);
		output_ = new LineSource(rawOutput_);
		CloseHandle(outputPipeWrite);
	}
	else {
		rawOutput_ = 0;
		output_ = 0;
	}
	
	if (ioPolicy_ & ForwardError) {
		rawError_ = new SystemStream(errorPipeRead);
		error_ = new LineSource(rawError_);
		CloseHandle(errorPipeWrite);
	}
	else {
		rawError_ = 0;
		error_ = 0;
	}
}

void Process::startDaemon()
{
	PONA_THROW(WindowsException, "Unsupported function");
}

void Process::terminate()
{
	kill();
}

void Process::hangup()
{
	kill();
}

void Process::kill()
{
	if (!TerminateProcess(handle_, 1))
		PONA_WINDOWS_EXCEPTION;
}

int Process::wait()
{
	if (WaitForSingleObject(handle_, INFINITE) == WAIT_FAILED)
		PONA_WINDOWS_EXCEPTION;
	DWORD exitCode = 0;
	if (!GetExitCodeProcess(handle_, &exitCode))
		PONA_WINDOWS_EXCEPTION;
	CloseHandle(handle_);
	return int(exitCode);
}

} // namespace pona
