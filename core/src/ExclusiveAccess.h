/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_EXCLUSIVEACCESS_H
#define FLUX_EXCLUSIVEACCESS_H

#include <flux/ExclusiveSection>

namespace flux {

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

} // namespace flux

#endif // FLUX_EXCLUSIVEACCESS_H
