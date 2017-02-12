/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/LineBuffer>

namespace cc {
namespace http {

class TapBuffer: public LineBuffer
{
public:
    static Ref<TapBuffer> open(Stream *stream, String prefix);

    virtual String prefix() const;

    void write(const ByteArray *buf);

private:
    TapBuffer(Stream *stream, String prefix);

    String prefix_;
};

}} // namespace cc::http
