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
    static Ref<Notice> create(CopyrightList *copyrights, const string &statement, Header *header = nullptr) {
        return new Notice{copyrights, statement, header};
    }

    CopyrightList *copyrights() const { return copyrights_; }
    string statement() const { return statement_; }
    Header *header() const { return header_; }

private:
    Notice(CopyrightList *copyrights, const string &statement, Header *header = nullptr):
        copyrights_{copyrights},
        statement_{statement->trim()},
        header_{header}
    {}

    Ref<CopyrightList> copyrights_;
    string statement_;
    Ref<Header> header_;
};

} // namespace ccclaim
