/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>
#include <cc/tar/ArchiveWriter>

namespace cc {
namespace tar {

class ArWriter: public ArchiveWriter
{
public:
    static Ref<ArWriter> open(Stream *sink);

    void writeFile(String path);

private:
    ArWriter(Stream *sink);

    Ref<Stream> sink_;
    bool firstTime_;
};

}} // namespace cc::tar

