/*
 * Copyright (C) 2025 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/WebSocketStream>
#include <cc/WebSocketPing>
#include <cc/WebSocketPong>
#include <cc/WebSocketClose>
#include <cc/ByteSource>
#include <cc/ByteSink>
#include <cc/System>
#include <cc/Random>

namespace cc {

struct WebSocketStream::State final: public Object::State
{
    explicit State(const Stream &stream, bool doMask):
        source_{stream, ByteOrder::BigEndian},
        sink_{stream, ByteOrder::BigEndian},
        doMask_{doMask}
    {}

    bool read(Out<String> message, Out<WebSocketMessage::Type> type)
    {
        List<String> pending;
        long pendingSize = 0;

        while (isOpen_)
        try {
            WebSocketFrame frame { source_, maxIncomingMessageSize_ };

            if (WebSocketPing ping { frame }; ping) {
                write(WebSocketPong::createFrame(ping));
                if (onPingReceived_) onPingReceived_();
            }
            else if (WebSocketPong pong { frame }; pong) {
                if (onPongReceived_) onPongReceived_();
            }
            else if (WebSocketClose close { frame }; close) {
                if (isOpen_) {
                    this->close(close.status());
                }
            }
            else if (frame.type() == WebSocketFrame::Type::Continuation) {
                if (pending.count() == 0) {
                    this->close(WebSocketStatus::ProtocolError);
                }
                else if (pendingSize + frame.payload().count() > maxIncomingMessageSize_) {
                    throw WebSocketFrame::FrameToBig{};
                }
                else {
                    pending.pushBack(frame.payload());
                    pendingSize += frame.payload().count();
                    if (frame.fin()) {
                        *message = String { pending };
                        pending.deplete();
                        pendingSize = 0;
                        break;
                    }
                }
            }
            else if (
                frame.type() == WebSocketFrame::Type::Binary ||
                frame.type() == WebSocketFrame::Type::Text
            ) {
                *type = frame.type();
                if (frame.fin()) {
                    *message = frame.payload();
                    break;
                }
                else {
                    pending.deplete();
                    pending.pushBack(frame.payload());
                    pendingSize = frame.payload().count();
                }
            }
        }
        catch (WebSocketFrame::FrameToBig &)
        {
            close(WebSocketStatus::MessageToBig);
        }

        return isOpen_;
    }

    bool write(const String &message, WebSocketMessage::Type type, uint32_t mask)
    {
        if (message.count() > maxOutgoingFramePayloadSize_) {
            String bigMessage = message;
            bool ok = true;
            for (long offset = 0; offset < bigMessage.count() && ok;) {
                long end = offset + maxOutgoingFramePayloadSize_;
                if (end > bigMessage.count()) end = bigMessage.count();
                const WebSocketFrame::Type frameType = (offset == 0) ? type : WebSocketFrame::Type::Continuation;
                const String frameData = bigMessage.select(offset, end);
                const bool fin = (end == bigMessage.count());
                ok = ok && write({frameType, frameData, fin}, mask);
                offset = end;
            }
            return ok;
        }
        return write(WebSocketFrame{type, message}, mask);
    }

    bool write(WebSocketFrame &&frame, uint32_t mask = 0)
    {
        if (isOpen_) {
            if (doMask_ && mask == 0) mask = nextMask();
            frame.writeTo(sink_, mask);
        }
        return isOpen_;
    }

    bool close(WebSocketStatus status)
    {
        if (isOpen_) status_ = status;
        bool ok = write(WebSocketClose::createFrame(status));
        isOpen_ = false;
        return ok;
    }

    bool ping()
    {
        return write(WebSocketPing::createFrame());
    }

    bool pong()
    {
        return write(WebSocketPong::createFrame());
    }

    uint32_t nextMask() const
    {
        if (!doMask_) return 0;
        uint32_t mask = 0;
        while (mask == 0) mask = System::noise();
        return mask;
    }

    ByteSource source_;
    ByteSink sink_;
    bool isOpen_ { true };
    bool doMask_ { false };
    long maxOutgoingFramePayloadSize_ { std::numeric_limits<long>::max() };
    long maxIncomingMessageSize_ { std::numeric_limits<long>::max() };
    WebSocketStatus status_ { WebSocketStatus::Ok };
    Function<void()> onPingReceived_;
    Function<void()> onPongReceived_;
};

WebSocketStream::WebSocketStream(const Stream &stream, Type type):
    Object{new State{stream, static_cast<bool>(type)}}
{}

WebSocketStream &WebSocketStream::maxOutgoingFramePayloadSize(long size)
{
    me().maxOutgoingFramePayloadSize_ = size;
    return *this;
}

WebSocketStream &WebSocketStream::maxIncomingMessageSize(long size)
{
    me().maxIncomingMessageSize_ = size;
    return *this;
}

WebSocketStream &WebSocketStream::onPingReceived(Function<void()> &&f)
{
    me().onPingReceived_ = std::move(f);
    return *this;
}

WebSocketStream &WebSocketStream::onPongReceived(Function<void()> &&f)
{
    me().onPongReceived_ = std::move(f);
    return *this;
}

bool WebSocketStream::read(Out<String> message, Out<WebSocketMessage::Type> type)
{
    return me().read(&message, &type);
}

bool WebSocketStream::write(const String &message, WebSocketMessage::Type type, uint32_t mask)
{
    return me().write(message, type, mask);
}

bool WebSocketStream::isOpen() const
{
    return me().isOpen_;
}

bool WebSocketStream::close(WebSocketStatus status)
{
    return me().close(status);
}

WebSocketStatus WebSocketStream::status() const
{
    return me().status_;
}

bool WebSocketStream::ping()
{
    return me().ping();
}

bool WebSocketStream::pong()
{
    return me().pong();
}

const WebSocketStream::State &WebSocketStream::me() const
{
    return Object::me.as<State>();
}

WebSocketStream::State &WebSocketStream::me()
{
    return Object::me.as<State>();
}

} // namespace cc
