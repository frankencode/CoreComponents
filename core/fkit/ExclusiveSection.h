/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_EXCLUSIVESECTION_H
#define FKIT_EXCLUSIVESECTION_H

namespace fkit
{

class ExclusiveAccess;

class ExclusiveSection
{
public:
	inline ExclusiveSection(): access_(0) {}

private:
	friend class ExclusiveAccess;

	inline bool acquire() { return __sync_bool_compare_and_swap(&access_, 0, 1); }
	inline void release() { access_ = 0; }

	bool access_;
};

} // namespace fkit

#endif // FKIT_EXCLUSIVESECTION_H
