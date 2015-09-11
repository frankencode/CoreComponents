/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/SpinLock>
#include <flux/Mutex>

using namespace flux;
using namespace flux::testing;

class TestFloat754: public TestCase
{
    void run()
    {
        float64_t values[] = {
            -1./3.,
            -0.55,
            0.49,
            15,
            -1.5,
            1.1111111111,
            1.1111111111111111111111111111111,
            1e-16,
            1e-308
        };

        for (unsigned i = 0; i < sizeof(values)/sizeof(double); ++i)
            fout("%%|\n") << values[i];

        float64_t one = 1., zero = 0.;

        fout("%%|\n") << one/zero;
        fout("%%|\n") << -one/zero;
        fout("%%|\n") << zero/zero;
        fout("%%|\n") << 0.;

        fout("bin(zero/zero) = %%\n") << hex(union_cast<uint64_t>(zero/zero));
        fout("bin(one/zero) = %%\n") << hex(union_cast<uint64_t>(one/zero));
        fout("bin(-one/zero) = %%\n") << hex(union_cast<uint64_t>(-one/zero));
        fout("bin(nan) = %%\n") << hex(union_cast<uint64_t>(zero/zero));

        FLUX_VERIFY(hex(union_cast<uint64_t>(zero/zero)) == "fff8000000000000");
        FLUX_VERIFY(hex(union_cast<uint64_t>(one/zero)) == "7ff0000000000000");
        FLUX_VERIFY(hex(union_cast<uint64_t>(-one/zero)) == "fff0000000000000");
    }
};

class TestSizes: public TestCase
{
    void run()
    {
        fout("sizeof(bool) = %%\n") << sizeof(bool);
        fout("sizeof(void *) = %%\n") << sizeof(void *);
        fout("sizeof(int) = %%\n") << sizeof(int);
        fout("sizeof(long) = %%\n") << sizeof(long);
        fout("sizeof(uint8_t) = %%\n") << sizeof(uint8_t);
        fout("sizeof(uint16_t) = %%\n") << sizeof(uint16_t);
        fout("sizeof(uint32_t) = %%\n") << sizeof(uint32_t);
        fout("sizeof(uint64_t) = %%\n") << sizeof(uint64_t);
        fout("sizeof(float32_t) = %%\n") << sizeof(float32_t);
        fout("sizeof(float64_t) = %%\n") << sizeof(float64_t);
        fout("unsignedMax, intMax = %%, %%\n") << unsignedMax << intMax;

        fout("sizeof(Object) = %%\n") << sizeof(Object);
        fout("sizeof(SpinLock) = %%\n") << sizeof(SpinLock);
        fout("sizeof(Mutex) = %%\n") << sizeof(Mutex);
        fout("sizeof(Ref<Object>) = %%\n") << sizeof(Ref<Object>);
        fout("sizeof(String) = %%\n") << sizeof(String);
        fout("sizeof(Format) = %%\n") << sizeof(Format);
        fout("sizeof(Variant) = %%\n") << sizeof(Variant);

        fout("sizeof(time_t) = %%\n") << sizeof(time_t);
        fout("sizeof(size_t) = %%\n") << sizeof(size_t);
        timeval tv;
        timespec ts;
        fout("sizeof(timeval.tv_sec) = %%\n") << sizeof(tv.tv_sec);
        fout("sizeof(timespec.ts_sec) = %%\n") << sizeof(ts.tv_sec);

        FLUX_VERIFY(sizeof(float32_t) == 4);
        FLUX_VERIFY(sizeof(float64_t) == 8);
        FLUX_VERIFY(sizeof(Ref<Object>) == sizeof(void *));
    }
};

int main(int argc, char **argv)
{
    FLUX_TESTSUITE_ADD(TestFloat754);
    FLUX_TESTSUITE_ADD(TestSizes);

    return testSuite()->run(argc, argv);
}
