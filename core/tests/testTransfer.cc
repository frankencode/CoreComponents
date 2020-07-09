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
    class Instance: public Stream::Instance
    {
    public:
        int read(CharArray *data) override
        {
            if (transmissionLeft_ <= 0) return 0;

            int n = data->count() - random_->get(0, data->count() / 2);
            if (n > transmissionLeft_) n = transmissionLeft_;
            transmissionLeft_ -= n;
            return randomBytes_->readSpan(mutate(data->select(0, n)));
        }

        int transmissionSize() const { return transmissionSize_; }

    private:
        friend class TestSource;

        Instance(int transmissionSize):
            transmissionSize_{transmissionSize},
            transmissionLeft_{transmissionSize}
        {}

        int transmissionSize_;
        int transmissionLeft_;
        RandomBytes randomBytes_;
        Random random_;
    };

    TestSource(int transmissionSize = 0x123456):
        Stream{new Instance{transmissionSize}}
    {}

    const Instance *operator->() const { return my<Instance>(); }
};

class TestSink: public Stream
{
public:
    class Instance: public Stream::Instance
    {
    public:
        void write(const CharArray *data) override
        {
            fout() << "Received " << data->count() << " bytes" << nl;
            bytesTransferred_ += data->count();
            CC_VERIFY(data->equals(randomBytes_->readSpan(data->count())));
        }

        int bytesTransferred() const { return bytesTransferred_; }

    private:
        friend class TestSink;

        Instance() = default;

        int bytesTransferred_ { 0 };
        RandomBytes randomBytes_;
    };

    TestSink():
        Stream{new Instance}
    {}

    const Instance *operator->() const { return my<Instance>(); }
};

class SimpleRandomTransmission: public TestCase
{
    void run()
    {
        TestSource source;
        TestSink sink;
        Transfer::start(source, sink)->waitComplete();
        CC_VERIFY(source->transmissionSize() == sink->bytesTransferred());
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(SimpleRandomTransmission);

    return TestSuite::instance()->run(argc, argv);
}
