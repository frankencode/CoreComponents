#ifndef FLUXCLAIM_NOTICEPARSER_H
#define FLUXCLAIM_NOTICEPARSER_H

#include <flux/Singleton.h>
#include "Notice.h"

namespace fclaim
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

} // namespace fclaim

#endif // FLUXCLAIM_NOTICEPARSER_H
