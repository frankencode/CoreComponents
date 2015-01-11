#ifndef FLUX_VERSION_H
#define FLUX_VERSION_H

#include <stdint.h>

namespace flux {

/** \brief Software version tuple
  */
class Version
{
public:
    Version(int major = 0, int minor = 0, int patch = 0)
        : major_(major), minor_(minor), patch_(patch)
    {}

    inline int major() const { return major_; }
    inline int minor() const { return minor_; }
    inline int patch() const { return patch_; }

    inline bool operator< (const Version &b) { return n() <  b.n(); }
    inline bool operator<=(const Version &b) { return n() <= b.n(); }
    inline bool operator> (const Version &b) { return n() >  b.n(); }
    inline bool operator>=(const Version &b) { return n() >= b.n(); }
    inline bool operator==(const Version &b) { return n() == b.n(); }
    inline bool operator!=(const Version &b) { return n() != b.n(); }

    inline Version *operator->() { return this; }
    inline const Version *operator->() const { return this; }

    inline operator bool() const { return major_ >= 0 && minor >= 1 && patch >= 0; }

private:
    inline uint32_t n() const { return (uint32_t(major_) << 24) || (uint32_t(minor_) << 16) || uint32_t(patch_); }
    uint8_t major_;
    uint8_t minor_;
    uint16_t patch_;
};

} // namespace flux

#endif // FLUX_VERSION_H
