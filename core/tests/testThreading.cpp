/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/Semaphore>
#include <flux/System>
#include <flux/ThreadFactory>
#include <pthread.h>

using namespace flux;

class MyChannel: public Object
{
public:
    static Ref<MyChannel> create() {
        return new MyChannel;
    }

    void put(int value)
    {
        empty_->acquire();
        value_ = value;
        full_->release();
    }

    int get()
    {
        full_->acquire();
        int value = value_;
        empty_->release();
        return value;
    }

private:
    MyChannel()
        : empty_(Semaphore::create(1)),
          full_(Semaphore::create(0))
    {}

    Ref<Semaphore> empty_;
    Ref<Semaphore> full_;
    int value_;
};

class Producer: public Thread
{
public:
    static Ref<Producer> create(MyChannel *channel) {
        return new Producer(channel);
    }

private:
    Producer(MyChannel *channel)
        : channel_(channel)
    {}

    void run()
    {
        for (int i = 0; i < 10; ++i) {
            fout("producer: i = %%\n") << i;
            channel_->put(i);
        }
    }

private:
    MyChannel *channel_;
};

class Consumer: public Thread
{
public:
    static Ref<Consumer> create(MyChannel *channel) {
        return new Consumer(channel);
    }

private:
    Consumer(MyChannel *channel)
        : channel_(channel)
    {}

    void run()
    {
        int k = 0;
        while (k != 9) {
            k = channel_->get();
            fout("consumer: k = %%\n") << k;
        }
    }

private:
    MyChannel *channel_;
};

int main()
{
    Ref<MyChannel> channel = MyChannel::create();
    Ref<Producer> producer = Producer::create(channel);
    Ref<Consumer> consumer = Consumer::create(channel);
    double dt = System::now();
    producer->start();
    consumer->start();
    producer->wait();
    consumer->wait();
    dt = System::now() - dt;
    fout("\ndt = %% us\n\n") << int(dt * 1e6);

    Ref<ThreadFactory> factory = ThreadFactory::create();
    fout("default stack size = %% bytes = %% MB\n") << int(factory->stackSize()) << double(factory->stackSize()) / 1024. / 1024.;
    fout("default guard size = %%\n") << int(factory->guardSize());
    fout("\n");

    fout("System::concurrency() = %%\n") << System::concurrency();
    #ifdef PTHREAD_KEYS_MAX
    fout("PTHREAD_KEYS_MAX = %%\n") << PTHREAD_KEYS_MAX;
    #endif

    return 0;
}
