/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_NODECONFIGPROTOCOL_H
#define FLUXNODE_NODECONFIGPROTOCOL_H

#include <flux/meta/MetaProtocol>

namespace flux { template<class> class Singleton; }

namespace fluxnode {

using namespace flux;

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

} // namespace fluxnode

#endif // FLUXNODE_NODECONFIGPROTOCOL_H
