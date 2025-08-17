/*
 * Copyright (C) 2025 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TimeWebSocketDelegate>
#include <cc/System>

namespace cc {

struct TimeWebSocketDelegate::State final: public WebSocketDelegate::State
{
    void onMessageReceived(const String &message, WebSocketMessage::Type type) override
    {
        String response = Format{"{\"time\": %% }"} << static_cast<int64_t>(System::now());
        webSocket().write(response);
    }
};

TimeWebSocketDelegate::TimeWebSocketDelegate():
    WebSocketDelegate{new State}
{}

} // namespace cc
