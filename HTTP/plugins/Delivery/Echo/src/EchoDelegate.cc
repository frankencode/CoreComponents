/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/EchoDelegate>

namespace cc {

struct EchoDelegate::State final: public HttpServiceDelegate::State
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
