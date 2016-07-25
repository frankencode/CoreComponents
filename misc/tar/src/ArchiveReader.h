/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>
#include <cc/tar/ArchiveEntry>

namespace cc {
namespace tar {

class BrokenArchive {
public:
    BrokenArchive(off_t offset, String reason):
        offset_(offset), reason_(reason)
    {}

    inline off_t offset() const { return offset_; }
    inline String reason() const { return reason_; }

private:
    off_t offset_;
    String reason_;
};

class UnsupportedArchiveFormat {};

class ArchiveReader: public Object
{
public:
    virtual bool readHeader(Ref<ArchiveEntry> *entry) = 0;
    virtual void readData(ArchiveEntry *entry, Stream *sink = 0) = 0;
    void skipData(ArchiveEntry *entry);
};

}} // namespace cc::tar
