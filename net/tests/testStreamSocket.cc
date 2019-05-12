/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/stdio>
#include <cc/debug>
#include <cc/Dir>
#include <cc/CleanupGuard>
#include <cc/Worker>
#include <cc/Semaphore>
#include <cc/net/StreamSocket>

using namespace cc;
using namespace cc::testing;
using namespace cc::net;

class LocalEchoTest: public TestCase
{
public:
    void run()
    {
        String dirPath = Dir::createTemp();
        CC_INSPECT(dirPath);
        CC_VERIFY(Dir::exists(dirPath));

        CleanupGuard cleanup{dirPath};

        String path = dirPath->extendPath("echo");
        auto address = SocketAddress::create(ProtocolFamily::Local, path);
        CC_INSPECT(address);

        {
            auto upAndRunning = Semaphore::create();
            auto echoServer = Worker::start([=]{
                try {
                    auto listeningSocket = StreamSocket::listen(address);
                    upAndRunning->release();
                    auto connectedSocket = listeningSocket->accept();
                    connectedSocket->transferTo(connectedSocket);
                }
                catch (Exception &ex) {
                    CC_INSPECT(ex);
                }
            });

            upAndRunning->acquire();

            auto echoClient = Worker::start([=]{
                auto connectedSocket = StreamSocket::connect(address);
                String message = "Hello, echo!";
                connectedSocket->write(message);
                String echo = connectedSocket->readSpan(message->count());
                CC_INSPECT(message);
                CC_INSPECT(echo);
                CC_VERIFY(message == echo);
            });
        }
    }
};

int main(int argc, char **argv)
{
    CC_TESTSUITE_ADD(LocalEchoTest);

    return TestSuite::instance()->run(argc, argv);
}
