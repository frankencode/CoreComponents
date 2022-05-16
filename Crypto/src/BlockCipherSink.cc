/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/BlockCipherSink>
#include <cc/NullStream>

namespace cc {

struct BlockCipherSink::State: public Stream::State
{
    State(const Stream &sink, const BlockCipher &cipher, const Stream &pad):
        sink_{sink},
        cipher_{cipher},
        pad_{pad},
        block_{Bytes::allocate(cipher.blockSize())}
    {
        if (!pad_) pad_ = NullStream{};
    }

    ~State()
    {
        if (pending_) {
            write(pad_.readSpan(cipher_.blockSize() - pending_.count()));
            pending_.fill(0);
        }
        block_.fill(0);
    }

    void write(const Bytes &buffer, long fill = -1) override
    {
        Bytes data = buffer;
        if (fill > 0) data = data.select(0, fill);
        else if (fill == 0) return;

        if (pending_) {
            data = Bytes{pending_, data};
            pending_.fill(0);
            pending_ = Bytes{};
        }

        const int n_b = cipher_.blockSize();
        long i = 0;

        for (; i + n_b <= data.count(); i += n_b) {
            cipher_.encode(data.select(i, i + n_b), &block_);
            sink_.write(block_);
        }

        if (i < data.count()) {
            pending_ = data.copy(i, data.count());
        }
    }

    Stream sink_;
    BlockCipher cipher_;
    Stream pad_;
    Bytes block_;
    Bytes pending_;
};

BlockCipherSink::BlockCipherSink(const Stream &sink, const BlockCipher &cipher, const Stream &pad):
    Stream{new State{sink, cipher, pad}}
{}

int BlockCipherSink::blockSize() const
{
    return me().cipher_.blockSize();
}

long BlockCipherSink::pendingCount() const
{
    return me().pending_.count();
}

const BlockCipherSink::State &BlockCipherSink::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
