/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_RANDOM_H
#define FLUX_RANDOM_H

#include <flux/generics>

namespace flux {

/** Random number generator as described by Lewis et al/1969
  * for the System/360. The generator is reentrant.
  */
class Random: public Object
{
public:
    inline static Ref<Random> open(int seed = -1) {
        return new Random(seed);
    }

    inline int max() const { return m_ - 1; }
    inline int period() const { return m_ - 1; }

    inline int get() {
        x_ = (16807 * x_) % m_; /* 7^5 == 16807 */
        return x_;
    }

    /** Return a random number in range [a, b].
      */
    inline int get(int a, int b) {
        FLUX_ASSERT(b <= m_ - 1);
        FLUX_ASSERT(a <= b);
        return (uint64_t(get()) * (b - a)) / (m_ - 1) + a;
    }

    inline bool read(int *x) {
        *x = get();
        return true;
    }

private:
    Random(int seed);

    enum { m_ = (1u << 31) - 1 };
    unsigned x_;
};

} // namespace flux

#endif // FLUX_RANDOM_H
