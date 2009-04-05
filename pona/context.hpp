/*
 * context.hpp -- querying the current execution context
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_CONTEXT_HPP
#define PONA_CONTEXT_HPP

#include <sys/types.h>
#include "atoms"
#include "String.hpp"
#include "Map.hpp"

namespace pona
{

void cd(String path);
String cwd();

bool isRelative(String path);
bool isAbsolute(String path);
String makeAbsolute(String path);
String fileName(String path);
String stripComponent(String path);

void setAccessMask(int mask);

uid_t realUserId();
gid_t realGroupId();
uid_t effectiveUserId();
gid_t effectiveGroupId();
bool isSuperUser();

String env(String key);
void setEnv(String key, String value);
void unsetEnv(String key);

typedef Map<String, String> EnvMap;
Ref<EnvMap, Owner> envMap();

pid_t pid();
pid_t ppid();

} // namespace pona

#endif // PONA_CONTEXT_HPP
