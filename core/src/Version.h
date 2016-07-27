/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <stdint.h>
#include <cc/String>

namespace cc {

class Variant;

/** \class Version Version.h cc/Version
  * \brief Software version tuple
  */
class Version
{
public:
    Version():
        major_(0), minor_(0), patch_(0)
    {}

    Version(int major, int minor, int patch = 0):
        major_(major), minor_(minor), patch_(patch)
    {}

    Version(const char* s);
    Version(const String &s);
    Version(const Variant &v);

    inline static bool isValid(Version v) { return major(v) > 0 || minor(v) > 0; }

    inline static int major(Version v) { return v.major_; }
    inline static int minor(Version v) { return v.minor_; }
    inline static int patch(Version v) { return v.patch_; }

    inline bool operator< (const Version &b) { return n() <  b.n(); }
    inline bool operator<=(const Version &b) { return n() <= b.n(); }
    inline bool operator> (const Version &b) { return n() >  b.n(); }
    inline bool operator>=(const Version &b) { return n() >= b.n(); }
    inline bool operator==(const Version &b) { return n() == b.n(); }
    inline bool operator!=(const Version &b) { return n() != b.n(); }

    inline Version *operator->() { return this; }
    inline const Version *operator->() const { return this; }

    inline operator String() const;

private:
    friend class Variant;

    void read(String s);

    static Version cast(uint32_t x) { return Version(x >> 24, (x >> 16) & 0xFF, x & 0xFFFF); }

    inline uint32_t n() const { return (uint32_t(major_) << 24) | (uint32_t(minor_) << 16) | uint32_t(patch_); }

    uint8_t major_;
    uint8_t minor_;
    uint16_t patch_;
};

String str(Version v);

inline Version::operator String() const { return str(*this); }

} // namespace cc
