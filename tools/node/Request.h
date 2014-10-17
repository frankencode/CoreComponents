/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_REQUEST_H
#define FLUXNODE_REQUEST_H

#include <flux/String>
#include <flux/Map>
#include <flux/Stream>

namespace fluxnode {

using namespace flux;

class Request: public Map<String, String>
{
public:
    inline String method() const { return method_; }
    inline String target() const { return target_; }
    inline String version() const { return version_; }
    inline String host() const { return host_; }

    inline String line() const { return line_; }
    inline double time() const { return time_; }

    inline Stream *payload() const { return payload_; }

private:
    friend class ClientConnection;
    inline static Ref<Request> create() { return new Request; }
    Request() {}

    String method_;
    String target_;
    String version_;
    String host_;

    String line_;
    double time_;

    Ref<Stream> payload_;
};

} // namespace fluxnode

#endif // FLUXNODE_REQUEST_H
