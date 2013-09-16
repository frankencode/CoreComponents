/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_UTILS_H
#define FNODE_UTILS_H

#include <fkit/Date.h>

namespace fnode
{

using namespace fkit;

const char *reasonPhraseByStatusCode(int statusCode);
String formatDate(Date *date);
Ref<Date> scanDate(String text, bool *ok = 0);

} // namespace fnode

#endif // FNODE_UTILS_H
