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
    static Ref<EchoDelegate> create(ServiceWorker *worker);

    void process(HttpRequest *request) override;

private:
    EchoDelegate(ServiceWorker *worker);
};

} // namespace ccnode
