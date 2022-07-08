/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/NetworkState>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    String interfaceName = "";
    if (argc > 1) interfaceName = argv[1];

    NetworkState state{interfaceName};

    fout() << "interface: " << state.interfaceName() << nl;
    if (state.interface()) {
        fout() << "up: " << bool(state.interface().flags() & NetworkInterfaceFlags::Up) << nl;
    }
    if (state.address()) {
        fout() << "address: " << state.address();
        if (state.networkMask() > 0)
            fout() << "/" << state.networkMask();
        fout() << nl;
    }
    if (state.gateway()) {
        fout() << "gateway: " << state.gateway() << nl;
    }

    return 0;
}
