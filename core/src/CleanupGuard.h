/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>

namespace cc {

/** \class CleanupGuard CleanupGuard.h cc/CleanupGuard
  * \ingroup file_system
  * \brief Cleanup a directory at the end of the current scope
  * \see UnlinkGuard, Dir::cleanup()
  */
class CleanupGuard: public Object {
public:
    /** \brief Create a CleanupGuard
      * \param path directory path
      * \return new object instance
      */
    static Ref<CleanupGuard> create(String path) { return new CleanupGuard(path); }

    /** \brief Low-level constructor
      * \param path file path
      *
      * The public constructor allows to post an CleanupGuard statically into a scope.
      */
    CleanupGuard(String path);

    /** \brief Low-level destructor
      */
    ~CleanupGuard();

private:
    String path_;
};

} // namespace cc
