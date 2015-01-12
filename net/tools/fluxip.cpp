#include <flux/stdio>
#include <flux/net/NetworkState>

using namespace flux;
using namespace flux::net;

int main(int argc, char **argv)
{
    String interfaceName = "";
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
