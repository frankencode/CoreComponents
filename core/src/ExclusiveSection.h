/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

namespace cc {

class ExclusiveAccess;

class ExclusiveSection
{
public:
    inline ExclusiveSection(): access_(0) {}

private:
    friend class ExclusiveAccess;

    inline bool acquire() { return __sync_bool_compare_and_swap(&access_, 0, 1); }
    inline void release() { access_ = 0; }

    bool access_;
};

} // namespace cc

