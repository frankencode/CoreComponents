/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/EchoDelegate>

namespace cc {

struct EchoDelegate::State: public HttpServiceDelegate::State
{
    void process(const HttpRequest &request) override
    {
        {
            Format echo = response().chunk();
            echo << request.method() << " " << request.uri() << " " << request.version() << "\r\n";
            for (const KeyValue<String> &header: request.header()) {
                echo << header.key() << ": " << header.value() << "\r\n";
            }
            echo << "\r\n";
        }
        request.payload().transferTo(response().payload());
    }
};

EchoDelegate::EchoDelegate():
    HttpServiceDelegate{new State}
{}

} // namespace cc
