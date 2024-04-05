#include <cc/j1939/tp/EndOfMessage>
#include <cc/j1939/tp/protocol>
#include <cc/j1939/etp/EndOfMessage>
#include <cc/j1939/etp/protocol>
#include <cc/VirtualCanBus>
#include <cc/Thread>
#include <cc/HexDump>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc::j1939;
    using namespace cc;

    TestCase {
        "TP.SimpleBroadcastTransmission",
        []{
            String message { "Hello, world!" };
            String receivedMessage;

            VirtualCanBus bus;

            Thread sender {[&bus, message]{
                CanMedia media = bus.connect();
                CC_ASSERT(media);
                tp::broadcast(media, /*payloadPgn =*/ 1234, /*src =*/ 1, message, /*delay =*/ 0);
            }};

            Thread receiver {[&bus, &sender, message, &receivedMessage]{
                CanMedia media = bus.connect();
                sender.start();
                CanFrame frame;
                CC_ASSERT(media.read(&frame));
                tp::BroadcastAnnounce bam{frame};
                CC_ASSERT(bam);
                tp::receiveBroadcast(media, bam, &receivedMessage, /*timeout =*/ 0);
                sender.wait();
                media.write(CanFrame{});
            }};

            Thread monitor {[&bus, &receiver]{
                CanMedia media = bus.connect();
                receiver.start();
                for (CanFrame frame; media.read(&frame);) {
                    if (!frame.canId()) break;
                    CC_INSPECT(frame);
                }
                receiver.wait();
            }};

            monitor.start();
            monitor.wait();

            CC_INSPECT(receivedMessage);
            CC_CHECK(receivedMessage == message);
        }
    };

    TestCase {
        "TP.SimpleTransmission",
        []{
            String message { "Hello, world!" };
            String receivedMessage;

            VirtualCanBus bus;

            Thread sender {[&bus, message]{
                CanMedia media = bus.connect();
                CC_ASSERT(media);
                tp::send(media, /*payloadPgn =*/ 1234, /*src =*/ 1, /*dst =*/ 2, message, /*delay =*/ 0);
            }};

            Thread receiver {[&bus, &sender, message, &receivedMessage]{
                CanMedia media = bus.connect();
                sender.start();
                CanFrame frame;
                CC_ASSERT(media.read(&frame));
                tp::RequestToSend rts{frame};
                CC_ASSERT(rts);
                tp::receive(media, rts, &receivedMessage, /*timeout =*/ 0);
                sender.wait();
            }};

            Thread monitor {[&bus, &receiver]{
                CanMedia media = bus.connect();
                receiver.start();
                for (CanFrame frame; media.read(&frame);) {
                    CC_INSPECT(frame);
                    if (tp::EndOfMessage{frame}) break;
                }
                receiver.wait();
            }};

            monitor.start();
            monitor.wait();

            CC_INSPECT(receivedMessage);
            CC_CHECK(receivedMessage == message);
        }
    };

    TestCase {
        "ETP.SimpleTransmission",
        []{
            String message = String::allocate(1786);
            for (long i = 0; i < message.count(); ++i) message.byteAt(i) = i & 0xFF;

            String receivedMessage;

            VirtualCanBus bus;

            Thread sender {[&bus, message]{
                CanMedia media = bus.connect();
                CC_ASSERT(media);
                etp::send(media, /*payloadPgn =*/ 1234, /*src =*/ 1, /*dst =*/ 2, message, /*delay =*/ 0);
            }};

            Thread receiver {[&bus, &sender, message, &receivedMessage]{
                CanMedia media = bus.connect();
                sender.start();
                CanFrame frame;
                CC_ASSERT(media.read(&frame));
                etp::RequestToSend rts{frame};
                CC_ASSERT(rts);
                etp::receive(media, rts, &receivedMessage, /*timeout =*/ 0);
                sender.wait();
            }};

            Thread monitor {[&bus, &receiver]{
                CanMedia media = bus.connect();
                receiver.start();
                for (CanFrame frame; media.read(&frame);) {
                    CC_INSPECT(frame);
                    if (etp::EndOfMessage{frame}) break;
                }
                receiver.wait();
            }};

            monitor.start();
            monitor.wait();

            CC_CHECK(receivedMessage == message);
        }
    };

    return TestSuite{argc, argv}.run();
}
