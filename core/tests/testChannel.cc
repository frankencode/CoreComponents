/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/Thread>
#include <cc/Channel>
#include <cc/System>
#include <cc/List>
#include <cc/Random>
#include <cc/stdio>

using namespace cc;
using namespace cc::testing;

class Consumer: public Thread
{
public:
    static Ref<Consumer> create(int id, const Channel<int> &channel, int amount) { return new Consumer{id, channel, amount}; }
    List<int> list() const { return list_; }

private:
    Consumer(int id, const Channel<int> &channel, int amount):
        id_{id},
        channel_{channel},
        amount_{amount}
    {}

    void run() override
    {
        while (amount_ > 0) {
            int x = channel_->pop();
            fout("consumer %%: consuming %%\n") << id_ << x;
            list_->append(x);
            --amount_;
        }
    }

    int id_;
    Channel<int> channel_;
    int amount_;
    List<int> list_;
};

class Producer: public Thread
{
public:
    static Ref<Producer> create(int id, const Channel<int> &channel, int amount) { return new Producer{id, channel, amount}; }
    List<int> list() const { return list_; }

private:
    Producer(int id, const Channel<int> &channel, int amount):
        id_{id},
        channel_{channel},
        amount_{amount},
        random_{amount}
    {}

    void run() override
    {
        while (amount_ > 0) {
            int x = random_->get();
            fout("producer %%: producing %%\n") << id_ << x;
            list_->append(x);
            channel_->push(x);
            --amount_;
        }
    }

    int id_;
    Channel<int> channel_;
    int amount_;
    Random random_;
    List<int> list_;
};

class ConsumerProducer: public TestCase
{
    void run() override
    {
        Channel<int> channel;
        Ref<Producer> p = Producer::create(1, channel, 8);
        Ref<Consumer> c = Consumer::create(1, channel, 8);

        double dt = System::now();
        c->start();
        p->start();
        c->wait();
        p->wait();
        dt = System::now() - dt;

        fout("\ndt = %% us\n\n") << int(dt * 1e6);

        CC_VERIFY(p->list() == c->list());
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(ConsumerProducer);

    return TestSuite::instance()->run(argc, argv);
}
