/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include "Notice.h"

namespace cc { template<class> class Singleton; }

namespace ccclaim {

using namespace cc;

class NoticeSyntax;

class NoticeParser: public Object
{
public:
    Ref<Notice> readNotice(Header *header) const;

private:
    friend class Singleton<NoticeParser>;

    NoticeParser();
    Ref<NoticeSyntax> noticeSyntax_;
};

const NoticeParser *noticeParser();

} // namespace ccclaim

