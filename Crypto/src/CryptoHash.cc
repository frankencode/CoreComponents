/*
 * Copyright (C) 2023 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/CryptoHash>
#include <cc/Md5HashSink>
#include <cc/Sha1HashSink>
#include <cc/Sha256HashSink>
#include <cc/Sha512HashSink>
#include <cc/Sha3HashSink>
#include <cc/TextError>
#include <cc/HexDump>
#include <cc/str>

namespace cc {

CryptoHash CryptoHash::read(const String &text)
{
    CryptoHash hash;
    List<String> parts = text.split(':');
    if (parts.count() == 2) {
        hash.method_ = readMethodName(parts[0]);
        hash.sum_ = HexDump::read(parts[1]);
    }
    return hash;
}

CryptoHashSink CryptoHash::openSink(Method method)
{
    CryptoHashSink sink;
    switch (method) {
        case Method::Sha3_512: sink = Sha3HashSink<512>{}; break;
        case Method::Sha3_384: sink = Sha3HashSink<384>{}; break;
        case Method::Sha3_256: sink = Sha3HashSink<256>{}; break;
        case Method::Sha3_224: sink = Sha3HashSink<224>{}; break;
        case Method::Sha2_512: sink = Sha512HashSink{}; break;
        case Method::Sha2_256: sink = Sha256HashSink{}; break;
        case Method::Sha1: sink = Sha1HashSink{}; break;
        case Method::Md5: sink = Md5HashSink{}; break;
        default: sink = Sha3HashSink<512>{}; break;
    };
    return sink;
}

CryptoHash CryptoHash::compute(Method method, const Stream &source)
{
    CryptoHashSink hashSink = openSink(method);
    Stream{source}.transferTo(hashSink);
    return {method, hashSink.finish()};
}

bool CryptoHash::operator==(const CryptoHash &other) const
{
    return method_ == other.method_ && sum_ == other.sum_;
}

std::strong_ordering CryptoHash::operator<=>(const CryptoHash &other) const
{
    std::strong_ordering order = method_ <=> other.method_;
    if (order == std::strong_ordering::equal) {
        order = sum_ <=> other.sum_;
    }
    return order;
}

String CryptoHash::toString() const
{
    return methodName(method_) + ":" + hex(sum_);
}

} // namespace cc
