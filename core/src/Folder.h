/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Source>
#include <cc/Stream>

namespace cc {

class Folder: public Source<String>
{
public:
    /** Open a file from this folder
      * \param name name of the file to open
      * \return new content stream
      */
    virtual Ref<Stream> openFile(String name) = 0;
};

} // namespace cc
