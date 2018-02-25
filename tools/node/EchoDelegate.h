/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "ServiceDelegate.h"

namespace ccnode {

class EchoDelegate: public ServiceDelegate
{
public:
    static Ref<EchoDelegate> create(ServiceWorker *worker) {
        return new EchoDelegate(worker);
    }

    virtual void process(HttpRequest *request)
    {
        {
            Format echo = response()->chunk();
            echo << request->method() << " " << request->uri() << " " << request->version() << "\r\n";
            for (int i = 0; i < request->count(); ++i)
                echo << request->keyAt(i) << ": " << request->valueAt(i) << "\r\n";
            echo << "\r\n";
        }
        Ref<ByteArray> buf = ByteArray::allocate(0x4000);
        while (true) {
            int n = request->payload()->read(mutate(buf));
            if (n == 0) break;
            response()->write(buf->select(0, n));
        }
    }

private:
    EchoDelegate(ServiceWorker *worker):
        ServiceDelegate(worker)
    {}
};

} // namespace ccnode
