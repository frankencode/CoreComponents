/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/types>
#include <cc/strings>
#include <cc/HashSum>

namespace cc {

/** \brief CRC-32 check sum generator
  */
class Crc32: public HashSum
{
public:
    enum { Size = 4 };

    Crc32(uint32_t seed = ~uint32_t(0)):
        crc_(seed)
    {}

    void feed(const void *buf, int bufFill);
    inline uint32_t sum() const { return crc_; }

    void write(const ByteArray *data);
    Ref<ByteArray> finish();

private:
    uint32_t crc_;
};

inline uint32_t crc32(const void *buf, int bufSize) {
    Crc32 crc;
    if (buf) crc.feed(buf, bufSize);
    return crc.sum();
}

inline uint32_t crc32(const char *s) {
    Crc32 crc;
    if (s) crc.feed(s, strlen(s));
    return crc.sum();
}

inline uint32_t crc32(String s) {
    Crc32 crc;
    if (s) crc.feed(s->bytes(), s->count());
    return crc.sum();
}

} // namespace cc
