/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/net/StreamSocket>

namespace cc {
namespace net {

class ConnectionManager: public Object
{
public:
    bool waitEstablished(double timeout);

private:

};

}} // namespace cc::net
