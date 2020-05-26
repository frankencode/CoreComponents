/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/net/NetworkState>

using namespace cc;
using namespace cc::net;

int main(int argc, char **argv)
{
    string interfaceName = "";
    if (argc > 1) interfaceName = argv[1];

    Ref<NetworkState> state = NetworkState::query(interfaceName);

    fout() << "interface: " << state->interfaceName() << nl;
    if (state->interface())
        fout() << "up: " << bool(state->interface()->flags() & NetworkInterface::Up) << nl;

    if (state->address()) {
        fout() << "address: " << state->address();
        if (state->networkMask() > 0)
            fout() << "/" << state->networkMask();
        fout() << nl;
    }

    if (state->gateway())
        fout() << "gateway: " << state->gateway() << nl;

    return 0;
}
