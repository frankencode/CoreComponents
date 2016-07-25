/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>

namespace cc {
namespace tar {

class ArchiveWriter: public Object
{
public:
    virtual void writeFile(String path) = 0;
};

}} // namespace cc::tar

