/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpServiceInstance>
#include <cstdlib>

namespace cc {

HttpServiceInstance::State::State(const MetaObject &config):
    serviceName_{config.className()},
    requestLimit_{config("request-limit").to<long>()},
    requestPayloadLimit_{config("request-payload-limit").to<long>()},
    host_{config("host").to<String>()},
    uri_{config("uri").to<String>()},
    tlsOptions_{config("security").to<MetaObject>()}
    #if 0
    TODO...
    errorLoggingInstance_{},
    accessLoggingInstance_{}
    #endif
{}

} // namespace cc
