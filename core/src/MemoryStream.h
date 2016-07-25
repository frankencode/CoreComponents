/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Stream>

namespace cc {

class MemoryStream: public Stream
{
public:
    static Ref<MemoryStream> open(ByteArray *buffer);

    int read(ByteArray *data) override;
    void write(const ByteArray *data) override;

private:
    MemoryStream(ByteArray *buffer);
    Ref<ByteArray> buffer_;
    int i_;
};

} // namespace cc
