/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_RESPONSE_H
#define FLUXNODE_RESPONSE_H

#include <flux/String>
#include <flux/Map>
#include <flux/Stream>

namespace flux {
namespace stream {
class TransferMeter;
}}

namespace fluxnode {

using namespace flux;
using namespace flux::stream;

class ClientConnection;
class ServiceWorker;

class Response: public Map<String, String>
{
public:
    static Ref<Response> create(ClientConnection *client);

    void status(int statusCode, String reasonPhrase = "");
    void header(String name, String value);
    void begin(ssize_t contentLength = -1);
    void write(String bytes);
    Format chunk(String pattern);
    Format chunk();
    void end();

private:
    friend class ServiceWorker;

    Response(ClientConnection *client);

    void writeHeader();
    Stream *payload();

    inline bool delivered() const { return headerWritten_; }
    inline int statusCode() const { return statusCode_; }
    size_t bytesWritten() const;

    Ref<ClientConnection> client_;
    bool headerWritten_;
    Ref<TransferMeter> payload_;
    int statusCode_;
    ssize_t contentLength_;
    size_t bytesWritten_;
    String reasonPhrase_;
};

} // namespace fluxnode

#endif // FLUXNODE_RESPONSE_H
