/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "Copyright.h"
#include "Header.h"

namespace ccclaim {

using namespace cc;

class Notice: public Object
{
public:
    static Ref<Notice> create(const CopyrightList &copyrights, const String &statement, Header *header = nullptr) {
        return new Notice{copyrights, statement, header};
    }

    CopyrightList copyrights() const { return copyrights_; }
    String statement() const { return statement_; }
    Header *header() const { return header_; }

private:
    Notice(const CopyrightList &copyrights, const String &statement, Header *header = nullptr):
        copyrights_{copyrights},
        statement_{statement->trim()},
        header_{header}
    {}

    CopyrightList copyrights_;
    String statement_;
    Ref<Header> header_;
};

} // namespace ccclaim
