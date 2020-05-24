/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/List>

namespace ccclaim {

using namespace cc;

class Copyright: public Object
{
public:
    static Ref<Copyright> create(const String &holder, int yearStart, int yearEnd) {
        return new Copyright{holder, yearStart, yearEnd};
    }

    String holder() const { return holder_; }
    int yearStart() const { return yearStart_; }
    int yearEnd() const { return yearEnd_; }

private:
    Copyright(const String &holder, int yearStart, int yearEnd):
        holder_{holder},
        yearStart_{yearStart},
        yearEnd_{yearEnd}
    {}

    String holder_;
    int yearStart_;
    int yearEnd_;
};

typedef List< Ref<Copyright> > CopyrightList;

} // namespace ccclaim
