/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXCLAIM_NOTICEPARSER_H
#define FLUXCLAIM_NOTICEPARSER_H

#include <flux/Singleton.h>
#include "Notice.h"

namespace fluxclaim
{

using namespace flux;

class NoticeSyntax;

class NoticeParser: public Object, public Singleton<NoticeParser>
{
public:
	Ref<Notice> readNotice(Header *header) const;

private:
	friend class Singleton<NoticeParser>;

	NoticeParser();
	Ref<NoticeSyntax> noticeSyntax_;
};

NoticeParser *noticeParser();

} // namespace fluxclaim

#endif // FLUXCLAIM_NOTICEPARSER_H
