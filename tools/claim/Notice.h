#ifndef FCLAIM_NOTICE_H
#define FCLAIM_NOTICE_H

#include <fkit/String.h>
#include "Header.h"
#include "Copyright.h"

namespace fclaim
{

using namespace fkit;

class Notice: public Object
{
public:
	inline static Ref<Notice> create(Header *header, CopyrightList *copyrights, String text) {
		return new Notice(header, copyrights, text);
	}

	inline Header *header() const { return header_; }
	inline CopyrightList *copyrights() const { return copyrights_; }
	inline String statement() const { return statement_; }

private:
	Notice(Header *header, CopyrightList *copyrights, String statement)
		: header_(header),
		  copyrights_(copyrights),
		  statement_(statement)
	{}

	Ref<Header> header_;
	Ref<CopyrightList> copyrights_;
	String statement_;
};

} // namespace fclaim

#endif // FCLAIM_NOTICE_H
