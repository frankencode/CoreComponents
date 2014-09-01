/*
 * Copyright (C) 2013-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXCLAIM_COPYRIGHT_H
#define FLUXCLAIM_COPYRIGHT_H

#include <flux/String.h>
#include <flux/List.h>

namespace fluxclaim {

using namespace flux;

class Copyright: public Object
{
public:
	inline static Ref<Copyright> create(String holder, int yearStart, int yearEnd) {
		return new Copyright(holder, yearStart, yearEnd);
	}

	inline String holder() const { return holder_; }
	inline int yearStart() const { return yearStart_; }
	inline int yearEnd() const { return yearEnd_; }

private:
	Copyright(String holder, int yearStart, int yearEnd)
		: holder_(holder),
		  yearStart_(yearStart),
		  yearEnd_(yearEnd)
	{}

	String holder_;
	int yearStart_;
	int yearEnd_;
};

typedef List< Ref<Copyright> > CopyrightList;

} // namespace fluxclaim

#endif // FLUXCLAIM_COPYRIGHT_H
