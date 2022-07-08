/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/EchoInstance>
#include <cc/EchoDelegate>

namespace cc {

struct EchoInstance::State: public HttpServiceInstance::State
{
    State(const MetaObject &config):
        HttpServiceInstance::State{config}
    {}

    HttpServiceDelegate createDelegate() const override
    {
        return EchoDelegate{};
    }
};

EchoInstance::EchoInstance(const MetaObject &config):
    HttpServiceInstance{new State{config}}
{}

} // namespace cc
