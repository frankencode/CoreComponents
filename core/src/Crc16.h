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

/** \class Crc16 Crc16.h cc/Crc16
  * \brief CRC-16 check sum generator, according to RFC 1171
  */
class Crc16: public HashSum
{
public:
    enum { Size = 2 };

    Crc16(uint16_t seed = ~uint16_t(0)):
        crc_(seed)
    {}

    void feed(const void *buf, int bufFill);
    inline uint16_t sum() const { return crc_; }

    void write(const ByteArray *data);
    Ref<ByteArray> finish();

private:
    uint16_t crc_;
};

inline uint16_t crc16(const void *buf, int bufSize) {
    Crc16 crc;
    if (buf) crc.feed(buf, bufSize);
    return crc.sum();
}

inline uint16_t crc16(const char *s) {
    Crc16 crc;
    if (s) crc.feed(s, strlen(s));
    return crc.sum();
}

inline uint16_t crc16(String s) {
    Crc16 crc;
    if (s) crc.feed(s->bytes(), s->count());
    return crc.sum();
}

} // namespace cc
