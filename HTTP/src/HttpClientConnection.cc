/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpClientConnection>
#include <cc/System>

namespace cc {

HttpClientConnection::State::State(const StreamSocket &stream, const SocketAddress &localAddress):
    stream_{stream},
    localAddress_{localAddress},
    priority_{0},
    originAddress_{stream.address().networkPrefix()},
    arrivalTime_{System::now()},
    departureTime_{arrivalTime_}
{}

void HttpClientConnection::updateDepartureTime()
{
    me().departureTime_ = System::now();
}

} // namespace cc
