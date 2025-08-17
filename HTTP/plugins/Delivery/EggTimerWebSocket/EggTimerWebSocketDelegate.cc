/*
 * Copyright (C) 2025 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/EggTimerWebSocketDelegate>
#include <cc/System>
#include <cc/Thread>
#include <cc/Format>
#include <cc/yason>

namespace cc {

struct EggTimerWebSocketDelegate::State final: public WebSocketDelegate::State
{
    void onMessageReceived(const String &message, WebSocketMessage::Type type) override
    {
        if (message.startsWith("SetTimer")) {
            if (long offset = 0; message.find('{', &offset)) {
                String payload = message.copy(offset, message.count());
                Variant value = yasonParse(payload);
                duration_ = value("duration").to<long>();
            }
        }
        else if (message.startsWith("GetTimer")) {
            webSocket().write(
                Format{"{\"duration\": %%}"} << duration_
            );
        }
        else if (message.startsWith("StartTimer")) {
            target_ = System::now() + duration_;
        }
        else if (message.startsWith("GetRemaining")) {
            double now = System::now();
            long remaining = static_cast<long>(target_ - now);
            if (remaining < 0) remaining = 0;
            webSocket().write(
                Format{"{\"remaining\": %%}"} << remaining
            );
        }
    }

    long duration_ { 3 * 60 };
    double target_ { 0 };
};

EggTimerWebSocketDelegate::EggTimerWebSocketDelegate():
    WebSocketDelegate{new State}
{}

} // namespace cc
