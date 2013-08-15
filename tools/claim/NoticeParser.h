#ifndef FCLAIM_NOTICEPARSER_H
#define FCLAIM_NOTICEPARSER_H

#include <fkit/Singleton.h>
#include "Notice.h"

namespace fclaim
{

using namespace fkit;

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

} // namespace fclaim

#endif // FCLAIM_NOTICEPARSER_H
