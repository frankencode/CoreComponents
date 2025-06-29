/*
 * Copyright (C) 2025 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/WebSocketStream>
#include <cc/CaptureSink>
#include <cc/ReplaySource>
#include <cc/HexDump>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "TextMessageSingleFrameToClient",
        []{
            const char *testMessage = "Hello, world!";

            CaptureSink sink;
            {
                WebSocketStream webSocket { sink, WebSocketStream::Type::ServerToClient };
                CC_CHECK(webSocket.write(testMessage, WebSocketMessage::Type::Text));
            }
            Bytes output = sink.writtenData();
            Bytes expectedOutput { 0x81, 0x0D, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x21 };
            HexDump{}.write(output);
            CC_CHECK(output == expectedOutput);

            ReplaySource source { expectedOutput };
            {
                WebSocketStream webSocket { source, WebSocketStream::Type::ServerToClient };
                String message;
                WebSocketMessage::Type type = WebSocketMessage::Type::Unknown;
                CC_CHECK(webSocket.read(&message, &type));
                CC_CHECK(message == testMessage);
                CC_CHECK(type == WebSocketMessage::Type::Text);
            }
        }
    };

    TestCase {
        "BinaryMessageSingleFrameToClient",
        []{
            const char *testMessage = "Hello, world!";

            CaptureSink sink;
            {
                WebSocketStream webSocket { sink, WebSocketStream::Type::ServerToClient };
                CC_CHECK(webSocket.write(testMessage, WebSocketMessage::Type::Binary));
            }
            Bytes output = sink.writtenData();
            Bytes expectedOutput { 0x82, 0x0D, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x21 };
            HexDump{}.write(output);
            CC_CHECK(output == expectedOutput);

            ReplaySource source { expectedOutput };
            {
                WebSocketStream webSocket { source, WebSocketStream::Type::ServerToClient };
                String message;
                WebSocketMessage::Type type = WebSocketMessage::Type::Unknown;
                CC_CHECK(webSocket.read(&message, &type));
                CC_CHECK(message == testMessage);
                CC_CHECK(type == WebSocketMessage::Type::Binary);
            }
        }
    };

    TestCase {
        "TextMessageSingleFrameToServer",
        []{
            const char *testMessage = "Hello, world!";
            const uint32_t mask = UINT32_C(0xFF00FF);

            CaptureSink sink;
            {
                WebSocketStream webSocket { sink, WebSocketStream::Type::ClientToServer };
                CC_CHECK(webSocket.write(testMessage, WebSocketMessage::Type::Text, mask));
            }
            Bytes output = sink.writtenData();
            Bytes expectedOutput { 0x81, 0x8D, 0x00, 0xFF, 0x00, 0xFF, 0x48, 0x9A, 0x6C, 0x93, 0x6F, 0xD3, 0x20, 0x88, 0x6F, 0x8D, 0x6C, 0x9B, 0x21 };
            HexDump{}.write(output);
            CC_CHECK(output == expectedOutput);

            ReplaySource source { expectedOutput };
            {
                WebSocketStream webSocket { source, WebSocketStream::Type::ClientToServer };
                String message;
                WebSocketMessage::Type type = WebSocketMessage::Type::Unknown;
                CC_CHECK(webSocket.read(&message, &type));
                CC_CHECK(message == testMessage);
                CC_CHECK(type == WebSocketMessage::Type::Text);
            }
        }
    };

    TestCase {
        "TextMessageMultipleFrameToClient",
        []{
            const char *testMessage = "Hello, world!";

            CaptureSink sink;
            {
                WebSocketStream webSocket { sink, WebSocketStream::Type::ServerToClient };
                webSocket.maxOutgoingFramePayloadSize(8);
                CC_CHECK(webSocket.write(testMessage, WebSocketMessage::Type::Text));
            }
            Bytes output = sink.writtenData();
            Bytes expectedOutput { 0x01, 0x08, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x2C, 0x20, 0x77, 0x80, 0x05, 0x6F, 0x72, 0x6C, 0x64, 0x21 };
            HexDump{}.write(output);
            CC_CHECK(output == expectedOutput);

            ReplaySource source { expectedOutput };
            {
                WebSocketStream webSocket { source, WebSocketStream::Type::ServerToClient };
                String message;
                WebSocketMessage::Type type = WebSocketMessage::Type::Unknown;
                CC_CHECK(webSocket.read(&message, &type));
                CC_CHECK(message == testMessage);
                CC_CHECK(type == WebSocketMessage::Type::Text);
            }
        }
    };

    TestCase {
        "TextMessageSingleFrameToBigToClient",
        []{
            const char *testMessage = "Hello, world!";

            CaptureSink sink;
            {
                WebSocketStream webSocket { sink, WebSocketStream::Type::ServerToClient };
                CC_CHECK(webSocket.write(testMessage, WebSocketMessage::Type::Text));
            }

            ReplaySource source { sink.writtenData() };
            {
                WebSocketStream webSocket { source, WebSocketStream::Type::ServerToClient };
                webSocket.maxIncomingMessageSize(7);
                String message;
                CC_CHECK(!webSocket.read(&message));
                CC_CHECK(message == "");
                CC_CHECK(webSocket.status() == WebSocketStatus::MessageToBig);
            }
        }
    };

    TestCase {
        "TextMessageMultipleFrameToBigToClient",
        []{
            const char *testMessage = "Hello, world!";

            CaptureSink sink;
            {
                WebSocketStream webSocket { sink, WebSocketStream::Type::ServerToClient };
                webSocket.maxOutgoingFramePayloadSize(8);
                CC_CHECK(webSocket.write(testMessage, WebSocketMessage::Type::Text));
            }

            ReplaySource source { sink.writtenData() };
            {
                WebSocketStream webSocket { source, WebSocketStream::Type::ServerToClient };
                webSocket.maxIncomingMessageSize(11);
                String message;
                CC_CHECK(!webSocket.read(&message));
                CC_CHECK(message == "");
                CC_CHECK(webSocket.status() == WebSocketStatus::MessageToBig);
            }
        }
    };

    return TestSuite{argc, argv}.run();
}
