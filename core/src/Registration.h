/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_REGISTRATION_H
#define FLUX_REGISTRATION_H

namespace flux {

/** \brief object registration during library initialization
  */
template<class Asset>
class Registration
{
public:
    Registration() {
        static bool firstTime = true;
        if (firstTime) new Asset;
        firstTime = false;
    }
};

} // namespace flux

#endif // FLUX_REGISTRATION_H
