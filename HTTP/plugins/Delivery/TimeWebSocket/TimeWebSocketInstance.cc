/*
 * Copyright (C) 2025 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/TimeWebSocketInstance>
#include <cc/TimeWebSocketDelegate>

namespace cc {

TimeWebSocketInstance::State::State(const MetaObject &config):
    HttpServiceInstance::State{config}
{}

HttpServiceDelegate TimeWebSocketInstance::State::createDelegate() const
{
    return TimeWebSocketDelegate{};
}

TimeWebSocketInstance::TimeWebSocketInstance(const MetaObject &config):
    HttpServiceInstance{new State{config}}
{}

const TimeWebSocketInstance::State &TimeWebSocketInstance::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
