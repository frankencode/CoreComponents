/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_REFGUARD_H
#define FLUX_REFGUARD_H

#include "Ref.h"

namespace flux
{

template<class T>
class RefGuard
{
public:
	RefGuard(Ref<T> *ref): ref_(ref) {}
	~RefGuard() { *ref_ = 0; }
private:
	Ref<T> *ref_;
};

} // namespace flux

#endif // FLUX_REFGUARD_H
