/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Date>

namespace cc {
namespace http {

const char *reasonPhraseByStatusCode(int statusCode);
String formatDate(Date *date);
Ref<Date> scanDate(String text, bool *ok = 0);

}} // namespace cc::http
