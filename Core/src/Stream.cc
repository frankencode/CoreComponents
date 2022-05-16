/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Stream>
#include <cc/NullStream>
#include <sys/uio.h> // iovec
#include <cassert>

namespace cc {

void Stream::State::write(const List<Bytes> &buffers)
{
    for (const auto &b: buffers)
        write(b, b.count());
}

void Stream::State::write(const struct iovec *iov, int iovcnt)
{
    long total = 0;
    for (int i = 0; i < iovcnt; ++i) {
        total += iov[i].iov_len;
    }

    Bytes buffer = Bytes::allocate(total);
    long offset = 0;
    for (int i = 0; i < iovcnt; ++i) {
        std::memcpy(buffer.items() + offset, iov[i].iov_base, iov[i].iov_len);
        offset += iov[i].iov_len;
    }

    write(buffer, total);
}

long long Stream::State::transferTo(const Stream &sink, long long count, const Bytes &buffer)
{
    Bytes buffer_{buffer};
    Stream sink_{sink};
    long long total = 0;

    while (count != 0) {
        long m = (count < 0 || buffer_.count() < count) ? buffer_.count() : count;
        long n = read(&buffer_, m);
        if (n == 0) break;
        sink_.write(buffer_, n);
        total += n;
        count -= n;
    }

    return total;
}

long long Stream::State::transferTo(const Stream &sink, long long count)
{
    const long n = defaultTransferUnit();
    Bytes buffer = Bytes::allocate((0 < count && count < n) ? count : n);
    return transferTo(sink, count, buffer);
}

long long Stream::State::skip(long long count)
{
    NullStream sink;
    return transferTo(sink, count);
}

void Stream::State::drain(const Bytes &auxBuffer)
{
    Bytes buffer = auxBuffer;
    if (!buffer) buffer = Bytes::allocate(defaultTransferUnit());
    NullStream sink;
    transferTo(sink, -1, buffer);
}

long Stream::State::readSpan(Out<Bytes> buffer)
{
    const long w = buffer().count();
    long m = 0;
    while (m < w) {
        auto s = buffer().select(m, w);
        long n = read(&s);
        if (n == 0) break;
        m += n;
    }
    return m;
}

String Stream::State::readSpan(long count)
{
    if (count == 0) return String{};
    if (count < 0) return readAll();
    String s = String::allocate(count, '\0');
    readSpan(&s);
    return s;
}

String Stream::State::readAll(const Bytes &auxBuffer)
{
    Bytes buffer = auxBuffer;
    if (!buffer) buffer = Bytes::allocate(defaultTransferUnit());
    List<Bytes> parts;
    while (true) {
        long n = read(&buffer);
        if (n == 0) break;
        if (n < buffer.count()) {
            parts.append(buffer.copy(0, n));
        }
        else {
            parts.append(buffer);
            buffer = Bytes::allocate(buffer.count());
        }
    }
    return parts;
}

} // namespace cc
