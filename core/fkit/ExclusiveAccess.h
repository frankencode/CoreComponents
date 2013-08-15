/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_EXCLUSIVEACCESS_H
#define FKIT_EXCLUSIVEACCESS_H

#include "ExclusiveSection.h"

namespace fkit
{

class ExclusiveAccess
{
public:
	inline ExclusiveAccess(ExclusiveSection *section)
		: section_(section),
		  access_(section->acquire())
	{}

	inline ~ExclusiveAccess()
	{
		section_->release();
	}

	inline operator bool() const { return access_; }

private:
	ExclusiveSection *section_;
	const bool access_;
};

} // namespace fkit

#endif // FKIT_EXCLUSIVEACCESS_H
