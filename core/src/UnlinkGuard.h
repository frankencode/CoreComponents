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

/** \class UnlinkGuard UnlinkGuard.h cc/UnlinkGuard
  * \ingroup file_system
  * \brief Automatically unlink a file at the end of the current scope
  */
class UnlinkGuard: public Object {
public:
    /** \brief Create an unlink guard
      * \param path file path
      * \return new object instance
      */
    static Ref<UnlinkGuard> create(String path) { return new UnlinkGuard(path); }

    /** \brief Low-level constructor
      * \param path file path
      */
    UnlinkGuard(String path);

    /** \brief Low-level destructor
      */
    ~UnlinkGuard();

private:

    String path_;
};

} // namespace cc
