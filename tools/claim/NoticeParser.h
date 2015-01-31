/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXCLAIM_NOTICEPARSER_H
#define FLUXCLAIM_NOTICEPARSER_H

#include "Notice.h"

namespace flux { template<class> class Singleton; }

namespace fluxclaim {

using namespace flux;

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

} // namespace fluxclaim

#endif // FLUXCLAIM_NOTICEPARSER_H
