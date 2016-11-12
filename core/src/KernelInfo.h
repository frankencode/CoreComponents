/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Object>

namespace cc {

class KernelInfoData;

/** \class KernelInfo KernelInfo.h cc/KernelInfo
  * \brief Query information about the operating system kernel
  * \see System
  */
class KernelInfo: public Object
{
public:
    /** Query kernel information
      * \return new object instance
      */
    static Ref<KernelInfo> query();

    String name() const; ///< operating system name
    String release() const; ///< release version
    String version() const; ///< detailed build information
    String machine() const; ///< system architecture

private:
    KernelInfo();

    Ref<KernelInfoData> data_;
};

} // namespace cc
