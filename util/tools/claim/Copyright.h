/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXCLAIM_COPYRIGHT_H
#define FLUXCLAIM_COPYRIGHT_H

#include <flux/String>
#include <flux/List>

namespace fluxclaim {

using namespace flux;

class Copyright: public Object
{
public:
    inline static Ref<Copyright> create(String holder, int yearStart, int yearEnd) {
        return new Copyright(holder, yearStart, yearEnd);
    }

    inline String holder() const { return holder_; }
    inline int yearStart() const { return yearStart_; }
    inline int yearEnd() const { return yearEnd_; }

private:
    Copyright(String holder, int yearStart, int yearEnd)
        : holder_(holder),
          yearStart_(yearStart),
          yearEnd_(yearEnd)
    {}

    String holder_;
    int yearStart_;
    int yearEnd_;
};

typedef List< Ref<Copyright> > CopyrightList;

} // namespace fluxclaim

#endif // FLUXCLAIM_COPYRIGHT_H
