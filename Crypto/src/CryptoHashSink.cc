/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/CryptoHashSink>
#include <sys/uio.h> // iovec

namespace cc {

void CryptoHashSink::State::write(const List<Bytes> &buffers)
{
    for (const Bytes &h: buffers) HashSink::State::write(h);
}

void CryptoHashSink::State::write(const struct iovec *iov, int iovcnt)
{
    Bytes h;
    for (int i = 0; i < iovcnt; ++i) {
        h.wrap(iov[i].iov_base, iov[i].iov_len);
        HashSink::State::write(h);
    }
}

} // namespace cc
