/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/JitterSource>
#include <cc/Channel>
#include <cc/System>
#include <cc/SystemError>
#include <cc/Thread>
#include <cc/Semaphore>
#include <cc/Mutex>
#include <cmath>
#include <time.h>

namespace cc {

struct JitterSource::State final: public Stream::State
{
    static constexpr int bufferSize = 4096;

    State(double rate):
        latency_{1 / rate},
        resolution_{getTimeResolution()},
        workerThreads_{Dim<>{System::concurrency()}}
    {
        bool produce = true;
        for (Thread &thread: workerThreads_) {
            thread = Thread{[this, produce]{ runWorker(produce); }};
            produce = false;
            thread.start();
        }
    }

    ~State()
    {
        shutdown_.release(workerThreads_.count() + 1);
        for (Thread &thread: workerThreads_) thread.wait();
    }

    static double doubleTime(const struct timespec &stamp)
    {
        return static_cast<double>(stamp.tv_sec) + 1e-9 * static_cast<double>(stamp.tv_nsec);
    }

    static double getTimeResolution()
    {
        struct timespec stamp;
        CC_SYSCALL(clock_getres(CLOCK_MONOTONIC, &stamp));
        return doubleTime(stamp);
    }

    static double getTime()
    {
        struct timespec stamp;
        CC_SYSCALL(clock_gettime(CLOCK_MONOTONIC, &stamp));
        return doubleTime(stamp);
    }

    void runWorker(bool produce)
    {
        do {
            double time = getTime();
            uint8_t ch = static_cast<uint8_t>(std::fmod(time, (0x255 * resolution_)) / resolution_);
            if (produce && buffer_.count() < bufferSize) {
                buffer_.pushBack(ch);
            }
        }
        while (!shutdown_.acquireBefore(System::now() + latency_));
    }

    long read(Out<Bytes> buffer, long maxFill) override
    {
        if (maxFill < 0) maxFill = buffer->count();
        for (long i = 0; i < maxFill; ++i) {
            buffer_.popFront(&(*buffer)[i]);
        }
        return maxFill;
    }

    double latency_;
    double resolution_;
    Channel<uint8_t> buffer_;
    long fill_ { 0 };
    Semaphore<int> ready_;
    Semaphore<int> shutdown_;
    Array<Thread> workerThreads_;
    Thread masterThread_;
};

JitterSource::JitterSource(double rate):
    Stream{new State{rate}}
{}

} // namespace cc
