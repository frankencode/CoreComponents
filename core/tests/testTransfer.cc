/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/StreamTap>
#include <cc/CaptureSink>
#include <cc/Transfer>
#include <cc/RandomBytes>
#include <cc/Random>

using namespace cc;
using namespace cc::testing;

class TestSource: public Stream
{
public:
    static Ref<TestSource> open(int transmissionSize = 0x123456)
    {
        return new TestSource(transmissionSize);
    }

    virtual int read(CharArray *data) override
    {
        if (transmissionLeft_ <= 0) return 0;

        int n = data->count() - random_->get(0, data->count() / 2);
        if (n > transmissionLeft_) n = transmissionLeft_;
        transmissionLeft_ -= n;
        return randomBytes_->readSpan(mutate(data->select(0, n)));
    }

    int transmissionSize() const { return transmissionSize_; }

private:
    TestSource(int transmissionSize):
        transmissionSize_(transmissionSize),
        transmissionLeft_(transmissionSize)
    {}

    int transmissionSize_;
    int transmissionLeft_;
    RandomBytes randomBytes_;
    Random random_;
};

class TestSink: public Stream
{
public:
    static Ref<TestSink> open() { return new TestSink; }

    virtual void write(const CharArray *data) override
    {
        fout() << "Received " << data->count() << " bytes" << nl;
        bytesTransferred_ += data->count();
        CC_VERIFY(data->equals(randomBytes_->readSpan(data->count())));
    }

    int bytesTransferred() const { return bytesTransferred_; }

private:
    TestSink():
        bytesTransferred_(0)
    {}

    int bytesTransferred_;
    RandomBytes randomBytes_;
};

class SimpleRandomTransmission: public TestCase
{
    void run()
    {
        Ref<TestSource> source = TestSource::open();
        Ref<TestSink> sink = TestSink::open();
        Transfer::start(source, sink)->waitComplete();
        CC_VERIFY(source->transmissionSize() == sink->bytesTransferred());
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SimpleRandomTransmission);

    return TestSuite::instance()->run(argc, argv);
}
