/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXCLAIM_NOTICE_H
#define FLUXCLAIM_NOTICE_H

#include <flux/String.h>
#include "Header.h"
#include "Copyright.h"

namespace fclaim
{

using namespace flux;

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

#endif // FLUXCLAIM_NOTICE_H
