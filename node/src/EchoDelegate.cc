/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/EchoDelegate>

namespace cc {
namespace node {

Ref<EchoDelegate> EchoDelegate::create(DeliveryWorker *worker)
{
    return new EchoDelegate{worker};
}

EchoDelegate::EchoDelegate(DeliveryWorker *worker):
    DeliveryDelegate{worker}
{}

void EchoDelegate::process(const HttpRequest *request)
{
    {
        Format echo = response()->chunk();
        echo << request->method() << " " << request->uri() << " " << request->version() << "\r\n";
        for (int i = 0; i < request->count(); ++i)
            echo << request->keyAt(i) << ": " << request->valueAt(i) << "\r\n";
        echo << "\r\n";
    }
    String buf = String::allocate(0x4000);
    while (true) {
        int n = request->payload()->read(mutate(buf));
        if (n == 0) break;
        response()->write(buf->select(0, n));
    }
}

}} // namespace cc::node
