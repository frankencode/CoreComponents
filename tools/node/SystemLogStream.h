/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <syslog.h>
#include <cc/Stream>

namespace ccnode {

using namespace cc;

class SystemLogStream: public Stream
{
public:
    static Ref<SystemLogStream> open(int priority);

    void write(const CharArray *data) override;
    void write(const StringList *parts) override;

private:
    SystemLogStream(int priority);
    int priority_;
};

} // namespace ccnode
