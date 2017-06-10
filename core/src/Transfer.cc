/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Exception>
#include <cc/CircularBuffer>
#include <cc/Channel>
#include <cc/Thread>
#include <cc/Transfer>

namespace cc {

typedef Channel< Ref<ByteArray>, CircularBuffer > TransferBufferQueue;

class TransferInputWorker: public Thread
{
public:
    static Ref<TransferInputWorker> start(TransferBufferQueue *freeQueue, TransferBufferQueue *writeQueue, Stream *source)
    {
        return (new TransferInputWorker(freeQueue, writeQueue, source))->Thread::start();
    }

    bool ok() const { return ok_; }
    String errorMessage() const { return errorMessage_; }

    void stop() { freeQueue_->pushFront(Ref<ByteArray>()); }

private:
    TransferInputWorker(TransferBufferQueue *freeQueue, TransferBufferQueue *writeQueue, Stream *source):
        freeQueue_(freeQueue),
        writeQueue_(writeQueue),
        source_(source),
        ok_(true)
    {}

    virtual void run() override
    {
        try {
            Ref<ByteArray> buffer;
            while (freeQueue_->popFront(&buffer)) {
                int n = source_->read(buffer);
                if (n == 0) break;
                writeQueue_->pushBack(buffer->select(0, n));
            }
            writeQueue_->pushBack(Ref<ByteArray>());
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

    void stop() { writeQueue_->pushFront(Ref<ByteArray>()); }

private:
    TransferOutputWorker(TransferBufferQueue *writeQueue, TransferBufferQueue *freeQueue, Stream *sink):
        writeQueue_(writeQueue),
        freeQueue_(freeQueue),
        sink_(sink),
        ok_(true)
    {}

    virtual void run() override
    {
        try {
            Ref<ByteArray> buffer;
            while (writeQueue_->popFront(&buffer)) {
                sink_->write(buffer);
                freeQueue_->pushBack(buffer->unselect());
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
        freeQueue->pushBack(ByteArray::allocate(bufferSize));

    inputWorker_ = TransferInputWorker::start(freeQueue, writeQueue, source);
    outputWorker_ = TransferOutputWorker::start(writeQueue, freeQueue, sink);
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
