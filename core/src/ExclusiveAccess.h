/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/ExclusiveSection>

namespace cc {

class ExclusiveAccess
{
public:
    inline ExclusiveAccess(ExclusiveSection *section)
        : section_(section),
          access_(section->acquire())
    {}

    inline ~ExclusiveAccess()
    {
        section_->release();
    }

    inline operator bool() const { return access_; }

private:
    ExclusiveSection *section_;
    const bool access_;
};

} // namespace cc

