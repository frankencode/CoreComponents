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
    inline static Ref<Notice> create(CopyrightList *copyrights, String statement, Header *header = nullptr) {
        return new Notice(copyrights, statement, header);
    }

    inline CopyrightList *copyrights() const { return copyrights_; }
    inline String statement() const { return statement_; }
    inline Header *header() const { return header_; }

private:
    Notice(CopyrightList *copyrights, String statement, Header *header = nullptr)
        : copyrights_(copyrights),
          statement_(statement->trim()),
          header_(header)
    {}

    Ref<CopyrightList> copyrights_;
    String statement_;
    Ref<Header> header_;
};

} // namespace ccclaim
