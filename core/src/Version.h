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
  * \ingroup misc
  * \brief Software version tuple
  */
class Version
{
public:
    /// Construct an invalid version version tuple (0, 0, 0)
    Version():
        major_(0), minor_(0), patch_(0)
    {}

    /// Construct a new version software version tuple (\a major, \a minor, \a patch)
    Version(int major, int minor, int patch = 0):
        major_(major), minor_(minor), patch_(patch)
    {}

    /// Construct a version from a C-string \a s
    Version(const char* s);

    /// Construct a version by parsing a string \a s
    Version(const String &s);

    /// Construct a version by parsing a variant \a v
    Version(const Variant &v);

    /// Check if software version \a v is valid
    inline static bool isValid(Version v) { return major(v) > 0 || minor(v) > 0; }

    /// Return the major version of the software version tuple \a v
    inline static int major(Version v) { return v.major_; }

    /// Return the minor version of the software version tuple \a v
    inline static int minor(Version v) { return v.minor_; }

    /// Return the patch version of the software version tuple \a v
    inline static int patch(Version v) { return v.patch_; }

    /// Compare if this object is lower than \a b
    inline bool operator< (const Version &b) { return n() <  b.n(); }

    /// Compare if this object is lower or equal than \a b
    inline bool operator<=(const Version &b) { return n() <= b.n(); }

    /// Compare if this object is greater than \a b
    inline bool operator> (const Version &b) { return n() >  b.n(); }

    /// Compare if this object is greater or equal than \a b
    inline bool operator>=(const Version &b) { return n() >= b.n(); }

    /// Compare if this object equals \a b
    inline bool operator==(const Version &b) { return n() == b.n(); }

    /// Compare if this object is unequal \a b
    inline bool operator!=(const Version &b) { return n() != b.n(); }

    /// Conersion to string operator
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

/// Stringify a software version tuple \a v
String str(Version v);

inline Version::operator String() const { return str(*this); }

} // namespace cc
