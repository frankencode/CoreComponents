/*
 * Copyright (C) 2025 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/WebSocketDelegate>
#include <cc/WebSocketStream>
#include <cc/base64>
#include <cc/sha1>
#include <cc/Casefree>
#include <cc/debugging>

namespace cc {

void WebSocketDelegate::State::process(const HttpRequest &request)
{
    if (
        request.version() <= "1.0" ||
        casefree(request.header("Upgrade")) != "websocket" ||
        !request.header("Connection").contains("Upgrade") ||
        base64DecodedLength(request.header("Sec-WebSocket-Key")) != 16 ||
        request.header("Sec-WebSocket-Version") != "13"
    ) {
        response().setStatus(HttpStatus::BadRequest, "Bad Request");
        response().transmit();
    }
    else {
        response().setStatus(HttpStatus::SwitchingProtocols);
        response().setHeader("Upgrade", "websocket");
        response().setHeader("Connection", "Upgrade");
        response().setHeader(
            "Sec-WebSocket-Accept",
            base64Encode(sha1(
                String {
                    request.header("Sec-WebSocket-Key") +
                    "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
                }
            ))
        );
        response().transmit();
    }
}

void WebSocketDelegate::State::upgrade(Stream &stream)
{
    webSocket_ = WebSocketStream { stream, WebSocketStream::ServerToClient };
    onConnectionEstablished();
    {
        String message;
        WebSocketMessage::Type type = WebSocketMessage::Type::Unknown;
        while (webSocket_.read(&message, &type)) {
            onMessageReceived(message, type);
        }
    }
    onConnectionClosed(webSocket_.status());
}

void WebSocketDelegate::State::onConnectionEstablished()
{}

void WebSocketDelegate::State::onConnectionClosed(WebSocketStatus status)
{}

void WebSocketDelegate::State::onMessageReceived(const String &message, WebSocketMessage::Type type)
{}

} // namespace cc
