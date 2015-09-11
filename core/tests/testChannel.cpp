/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/testing/TestSuite>
#include <flux/stdio>
#include <flux/Thread>
#include <flux/Channel>
#include <flux/List>
#include <flux/Random>
#include <flux/System>

using namespace flux;
using namespace flux::testing;

typedef Channel<int> MyChannel;
typedef List<int> IntList;

class Consumer: public Thread
{
public:
    static Ref<Consumer> create(int id, MyChannel *channel, int amount) { return new Consumer(id, channel, amount); }
    inline Ref<IntList> list() const { return list_; }

private:
    Consumer(int id, MyChannel *channel, int amount)
        : id_(id),
          channel_(channel),
          amount_(amount),
          list_(IntList::create())
    {}

    void run()
    {
        while (amount_ > 0) {
            int x = channel_->pop();
            fout("consumer %%: consuming %%\n") << id_ << x;
            list_->append(x);
            --amount_;
        }
    }

    int id_;
    Ref<MyChannel> channel_;
    int amount_;
    Ref<IntList> list_;
};

class Producer: public Thread
{
public:
    static Ref<Producer> create(int id, MyChannel *channel, int amount) { return new Producer(id, channel, amount); }
    inline Ref<IntList> list() const { return list_; }

private:
    Producer(int id, MyChannel *channel, int amount)
        : id_(id),
          channel_(channel),
          amount_(amount),
          random_(Random::open(amount)),
          list_(IntList::create())
    {}

    void run()
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
    Ref<MyChannel> channel_;
    int amount_;
    Ref<Random> random_;
    Ref<IntList> list_;
};

class ConsumerProducer: public TestCase
{
    void run()
    {
        Ref<MyChannel> channel = MyChannel::create();
        Ref<Producer> p = Producer::create(1, channel, 8);
        Ref<Consumer> c = Consumer::create(1, channel, 8);

        double dt = System::now();
        c->start();
        p->start();
        c->wait();
        p->wait();
        dt = System::now() - dt;

        fout("\ndt = %% us\n\n") << int(dt * 1e6);

        FLUX_VERIFY(*p->list() == *c->list());
    }
};

int main(int argc, char **argv)
{
    FLUX_TESTSUITE_ADD(ConsumerProducer);

    return testSuite()->run(argc, argv);
}
