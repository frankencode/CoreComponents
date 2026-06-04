/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/HttpServiceDelegate>
#include <cc/HttpServiceWorkerState>

namespace cc {

const HttpServerConfig &HttpServiceDelegate::State::nodeConfig() const
{
    return workerState_->nodeConfig_;
}

const HttpServiceInstance &HttpServiceDelegate::State::serviceInstance() const
{
    return workerState_->serviceInstance_;
}

const HttpLoggingServiceInstance &HttpServiceDelegate::State::errorLoggingInstance() const
{
    return serviceInstance().errorLoggingInstance();
}

HttpClientConnection &HttpServiceDelegate::State::client()
{
    return workerState_->client_;
}

HttpResponseGenerator &HttpServiceDelegate::State::response()
{
    return workerState_->response_;
}

} // namespace cc
