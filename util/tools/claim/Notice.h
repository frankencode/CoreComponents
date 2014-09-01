/*
 * Copyright (C) 2013-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXCLAIM_NOTICE_H
#define FLUXCLAIM_NOTICE_H

#include "Copyright.h"
#include "Header.h"

namespace fluxclaim {

using namespace flux;

class Notice: public Object
{
public:
	inline static Ref<Notice> create(CopyrightList *copyrights, String statement, Header *header = 0) {
		return new Notice(copyrights, statement, header);
	}

	inline CopyrightList *copyrights() const { return copyrights_; }
	inline String statement() const { return statement_; }
	inline Header *header() const { return header_; }

private:
	Notice(CopyrightList *copyrights, String statement, Header *header = 0)
		: copyrights_(copyrights),
		  statement_(statement->trim()),
		  header_(header)
	{}

	Ref<CopyrightList> copyrights_;
	String statement_;
	Ref<Header> header_;
};

} // namespace fluxclaim

#endif // FLUXCLAIM_NOTICE_H
