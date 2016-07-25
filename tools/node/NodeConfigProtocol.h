/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/meta/MetaProtocol>

namespace cc { template<class> class Singleton; }

namespace ccnode {

using namespace cc;
using namespace cc::meta;

class NodeConfigProtocol: public MetaProtocol
{
public:
    void registerService(MetaObject *configPrototype);

private:
    friend class Singleton<NodeConfigProtocol>;
    NodeConfigProtocol();
    Ref<MetaProtocol> nodeProtocol_;
};

NodeConfigProtocol *configProtocol();

} // namespace ccnode

