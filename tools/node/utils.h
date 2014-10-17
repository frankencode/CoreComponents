/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_UTILS_H
#define FLUXNODE_UTILS_H

#include <flux/Date>

namespace fluxnode {

using namespace flux;

const char *reasonPhraseByStatusCode(int statusCode);
String formatDate(Date *date);
Ref<Date> scanDate(String text, bool *ok = 0);

} // namespace fluxnode

#endif // FLUXNODE_UTILS_H
