/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "DeliveryDelegate.h"

namespace ccnode {

class EchoDelegate: public DeliveryDelegate
{
public:
    static Ref<EchoDelegate> create(DeliveryWorker *worker);

    void process(HttpRequest *request) override;

private:
    EchoDelegate(DeliveryWorker *worker);
};

} // namespace ccnode
