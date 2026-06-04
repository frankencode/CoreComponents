/*
 * Copyright (C) 2025 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/EggTimerWebSocketInstance>
#include <cc/EggTimerWebSocketDelegate>

namespace cc {

EggTimerWebSocketInstance::State::State(const MetaObject &config):
    HttpServiceInstance::State{config}
{}

HttpServiceDelegate EggTimerWebSocketInstance::State::createDelegate() const
{
    return EggTimerWebSocketDelegate{};
}

EggTimerWebSocketInstance::EggTimerWebSocketInstance(const MetaObject &config):
    HttpServiceInstance{new State{config}}
{}

const EggTimerWebSocketInstance::State &EggTimerWebSocketInstance::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
