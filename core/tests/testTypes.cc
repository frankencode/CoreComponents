/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/SpinLock>
#include <cc/Mutex>

using namespace cc;
using namespace cc::testing;

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

        CC_VERIFY(hex(union_cast<uint64_t>(zero/zero)) == "FFF8000000000000");
        CC_VERIFY(hex(union_cast<uint64_t>(one/zero)) == "7FF0000000000000");
        CC_VERIFY(hex(union_cast<uint64_t>(-one/zero)) == "FFF0000000000000");
    }
};

class TestSizes: public TestCase
{
    class BucketHeader: public Mutex
    {
    public:
        uint16_t bucketIndex_;
        uint16_t bytesDirty_;
        uint16_t objectCount_;
        bool open_;
    };

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
        // fout("sizeof(mtx_t) = %%\n") << sizeof(mtx_t);
        fout("sizeof(BucketHeader) = %%\n") << sizeof(BucketHeader);
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

        CC_VERIFY(sizeof(float32_t) == 4);
        CC_VERIFY(sizeof(float64_t) == 8);
        CC_VERIFY(sizeof(Ref<Object>) == sizeof(void *));
    }
};

/*class TestCasts: public TestCase
{
    class A: public Object { public: static Ref<A> create() { return new A; }; private: A() {}; }
    class B: public Object { public: static Ref<B> create() { return new B; }; private: B() {}; }

    void run() {
        auto a = A::create();
        Object::cast<B *>(a);
    }
};*/

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(TestFloat754);
    CC_TESTSUITE_ADD(TestSizes);

    return TestSuite::instance()->run(argc, argv);
}
