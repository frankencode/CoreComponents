/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_SOCKETPAIR_H
#define FLUX_SOCKETPAIR_H

#include <flux/SystemStream>

namespace flux {

class SocketPair: public Object
{
public:
    inline static Ref<SocketPair> create() { return new SocketPair; }
    SystemStream *at(int i) const;

private:
    SocketPair();
    Ref<SystemStream> first_;
    Ref<SystemStream> second_;
};

} // namespace flux

#endif // FLUX_SOCKETPAIR_H
