/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/ModuloPadding>

namespace cc {

void ModuloPadding::writePadding(Out<BlockCipherSink> sink)
{
    Bytes padding = Bytes::allocate(sink->blockSize() - sink->pendingCount() % sink->blockSize());
    padding.fill(padding.count());
    sink->write(padding);
}

void ModuloPadding::removePadding(InOut<Bytes> message)
{
    if (message->count() > 0) {
        long paddingCount = message->at(message->count() - 1);
        if (paddingCount >= message->count()) {
            message->truncate(message->count() - paddingCount);
        }
    }
}

} // namespace cc
