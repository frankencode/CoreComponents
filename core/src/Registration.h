/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

namespace cc {

/** \class Registration Registration.h cc/Registration
  * \brief %Object registration during library initialization
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

#define CC_REGISTRATION(Type) namespace { Registration<Type> registrationOf ## Type; }

} // namespace cc
