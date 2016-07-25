/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Object>

namespace cc {

/** \class Random Random.h cc/Random
  * \brief Pseudo-random number generator
  *
  * The Random class provides a simple and fast pseudo-random number generator.
  * The generated numbers lay in the range min() to max(). The series will repeat
  * after period() numbers. Any choosen seed value will yield the same period.
  *
  * \see RandomSource
  */
class Random: public Object
{
public:
    /** Create a new random number generator.
      * \param seed seed number
      * \return new object instance
      */
    inline static Ref<Random> open(int seed = -1) {
        return new Random(seed);
    }

    /// Minimum random number
    inline int min() const { return 0; }

    /// Maximum random number
    inline int max() const { return m_ - 1; }

    /// Number of random numbers until the series repeats itself
    inline int period() const { return m_ - 1; }

    /// Get another random number in the range min() to max()
    inline int get() {
        x_ = (16807 * x_) % m_; /* 7^5 == 16807 */
        return x_;
    }

    /** Get another random number in the range a to b
      * \param a minimum random number
      * \param b maximum random number
      * \return next random number
      */
    inline int get(int a, int b) {
        CC_ASSERT(b <= m_ - 1);
        CC_ASSERT(a <= b);
        return (uint64_t(get()) * (b - a)) / (m_ - 1) + a;
    }

private:
    Random(int seed);

    enum { m_ = (1u << 31) - 1 };
    unsigned x_;
};

} // namespace cc
