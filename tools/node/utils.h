/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_UTILS_H
#define FLUXNODE_UTILS_H

#include <flux/Date.h>

namespace fnode
{

using namespace flux;

const char *reasonPhraseByStatusCode(int statusCode);
String formatDate(Date *date);
Ref<Date> scanDate(String text, bool *ok = 0);

} // namespace fnode

#endif // FLUXNODE_UTILS_H
