/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_SYSTEM_H
#define FLUX_SYSTEM_H

namespace flux {

class System
{
public:
    static int concurrency();
    static int pageSize();
    static double now();
};

} // namespace flux

#endif // FLUX_SYSTEM_H
