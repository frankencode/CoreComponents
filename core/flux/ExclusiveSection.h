/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_EXCLUSIVESECTION_H
#define FLUX_EXCLUSIVESECTION_H

namespace flux
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

} // namespace flux

#endif // FLUX_EXCLUSIVESECTION_H
