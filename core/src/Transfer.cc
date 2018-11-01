/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Exception>
#include <cc/CircularChannel>
#include <cc/Channel>
#include <cc/Thread>
#include <cc/Transfer>

namespace cc {

typedef CircularChannel<String> TransferBufferQueue;

class TransferInputWorker: public Thread
{
public:
    static Ref<TransferInputWorker> start(TransferBufferQueue *freeQueue, TransferBufferQueue *writeQueue, Stream *source)
    {
        return (new TransferInputWorker(freeQueue, writeQueue, source))->Thread::start();
    }

    bool ok() const { return ok_; }
    String errorMessage() const { return errorMessage_; }

    off_t totalRead() const { Guard<Mutex> guard(mutex_); return totalRead_; }

    void stop() { freeQueue_->pushFront(String()); }

private:
    TransferInputWorker(TransferBufferQueue *freeQueue, TransferBufferQueue *writeQueue, Stream *source):
        freeQueue_(freeQueue),
        writeQueue_(writeQueue),
        source_(source),
        mutex_(Mutex::create()),
        totalRead_(0),
        ok_(true)
    {}

    virtual void run() override
    {
        try {
            String buffer;
            while (freeQueue_->popFront(&buffer)) {
                int n = source_->read(mutate(buffer));
                if (n == 0) break;
                {
                    Guard<Mutex> guard(mutex_);
                    totalRead_ += n;
                }
                writeQueue_->pushBack(buffer->select(0, n));
            }
            writeQueue_->pushBack(String());
        }
        catch (Exception &ex) {
            errorMessage_ = ex.message();
            ok_ = false;
        }
        catch (...) {
            ok_ = false;
        }
    }

    Ref<TransferBufferQueue> freeQueue_;
    Ref<TransferBufferQueue> writeQueue_;
    Ref<Stream> source_;
    Ref<Mutex> mutex_;
    off_t totalRead_;
    bool ok_;
    String errorMessage_;
};

class TransferOutputWorker: public Thread
{
public:
    static Ref<TransferOutputWorker> start(TransferBufferQueue *writeQueue, TransferBufferQueue *freeQueue, Stream *sink)
    {
        return (new TransferOutputWorker(writeQueue, freeQueue, sink))->Thread::start();
    }

    bool ok() const { return ok_; }
    String errorMessage() const { return errorMessage_; }

    off_t totalWritten() const { Guard<Mutex> guard(mutex_); return totalWritten_; }

    void stop() { writeQueue_->pushFront(String()); }

private:
    TransferOutputWorker(TransferBufferQueue *writeQueue, TransferBufferQueue *freeQueue, Stream *sink):
        writeQueue_(writeQueue),
        freeQueue_(freeQueue),
        sink_(sink),
        mutex_(Mutex::create()),
        totalWritten_(0),
        ok_(true)
    {}

    virtual void run() override
    {
        try {
            String buffer;
            while (writeQueue_->popFront(&buffer)) {
                sink_->write(buffer);
                {
                    Guard<Mutex> guard(mutex_);
                    totalWritten_ += buffer->count();
                }
                freeQueue_->pushBack(buffer->parent());
            }
        }
        catch (Exception &ex) {
            errorMessage_ = ex.message();
            ok_ = false;
        }
        catch (...) {
            ok_ = false;
        }
    }

    Ref<TransferBufferQueue> writeQueue_;
    Ref<TransferBufferQueue> freeQueue_;
    Ref<Stream> sink_;
    Ref<Mutex> mutex_;
    off_t totalWritten_;
    bool ok_;
    String errorMessage_;
};

Ref<Transfer> Transfer::start(Stream *source, Stream *sink, int bufferSize, int bufferCount)
{
    return new Transfer(source, sink, bufferSize, bufferCount);
}

Transfer::Transfer(Stream *source, Stream *sink, int bufferSize, int bufferCount)
{
    Ref<TransferBufferQueue> freeQueue = TransferBufferQueue::create(bufferCount);
    Ref<TransferBufferQueue> writeQueue = TransferBufferQueue::create(bufferCount);

    for (int i = 0; i < bufferCount; ++i)
        freeQueue->pushBack(CharArray::allocate(bufferSize));

    inputWorker_ = TransferInputWorker::start(freeQueue, writeQueue, source);
    outputWorker_ = TransferOutputWorker::start(writeQueue, freeQueue, sink);
}

off_t Transfer::totalRead() const
{
    return inputWorker_->totalRead();
}

off_t Transfer::totalWritten() const
{
    return outputWorker_->totalWritten();
}

void Transfer::stop()
{
    inputWorker_->stop();
    outputWorker_->stop();
}

void Transfer::waitComplete()
{
    inputWorker_->wait();
    outputWorker_->wait();
    if (!inputWorker_->ok()) throw TransferError(inputWorker_->errorMessage());
    if (!outputWorker_->ok()) throw TransferError(outputWorker_->errorMessage());
}

} // namespace cc
