/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/FileStatus>
#include "HttpRequest.h"

namespace ccnode {

class ScriptHandler: public Object
{
public:
    virtual bool process(HttpRequest *request, FileStatus *status, const String &documentRoot = "") = 0;
};

} // namespace ccnode
