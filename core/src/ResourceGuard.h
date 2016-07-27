/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>

namespace cc {

/** \class ResourceGuard ResourceGuard.h cc/ResourceGuard
  * \brief Resource context guard
  * \see ResourceContext
  */
class ResourceGuard
{
public:
    ResourceGuard(String resource);
    ~ResourceGuard();
};

} // namespace cc
