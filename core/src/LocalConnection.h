/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_LOCALCONNECTION_H
#define FLUX_LOCALCONNECTION_H

#include <flux/SystemStream>

namespace flux {

/** \brief a pair of connceted local sockets
  */
class LocalConnection: public Object
{
public:
    inline static Ref<LocalConnection> create() { return new LocalConnection; }
    Ref<SystemStream> openFirst() const { return SystemStream::create(fd_[0]); }
    Ref<SystemStream> openSecond() const { return SystemStream::create(fd_[1]); }

private:
    LocalConnection();
    int fd_[2];
};

} // namespace flux

#endif // FLUX_LOCALCONNECTION_H
