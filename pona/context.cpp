/*
 * context.cpp -- querying the current execution context
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> // chdir, getcwd
#include <errno.h>
#include "context.hpp"

extern "C" char** environ;

namespace pona
{

void cd(String path)
{
	if (::chdir(path.utf8()) == -1)
		PONA_SYSTEM_EXCEPTION;
}

String cwd()
{
	char* cs = ::getcwd(0, 0);
	if (!cs)
		PONA_SYSTEM_EXCEPTION;
	String path(cs);
	::free(cs);
	return path;
}

void setAccessMask(int mask) { ::umask(mask); }

uid_t realUserId() { return ::getuid(); }
gid_t realGroupId() { return ::getgid(); }
uid_t effectiveUserId() { return ::geteuid(); }
gid_t effectiveGroupId() { return ::getegid(); }

bool isSuperUser() { return (::geteuid() == 0) || (::getegid() == 0); }

String env(String key)
{
	return getenv(key.utf8());
}

void setEnv(String key, String value)
{
	if (setenv(key.utf8(), value.utf8(), 1) == -1)
		PONA_SYSTEM_EXCEPTION;
}

void unsetEnv(String key)
{
	errno = 0;
	unsetenv(key.utf8());
	if (errno != 0)
		PONA_SYSTEM_EXCEPTION;
}

typedef Map<String, String> EnvMap;
Ref<EnvMap, Owner> envMap()
{
	char** env = ::environ;
	Ref<EnvMap, Owner> map = new EnvMap;
	int i = 0;
	while (env[i] != 0) {
		String s(env[i]);
		int k = s->find(String("="));
		if (k != s->length()) {
			String key = s->copy(0, k);
			String value = s->copy(k + 1, s->length() - (k + 1));
			map->set(key, value);
		}
		++i;
	}
	return map;
}

pid_t pid() { return getpid(); }
pid_t ppid() { return getppid(); }

} // namespace pona
