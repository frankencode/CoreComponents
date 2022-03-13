/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ServerSocket>
#include <cc/ClientSocket>
#include <cc/ScopeGuard>
#include <cc/File>
#include <cc/Dir>
#include <cc/Thread>
#include <cc/Semaphore>
#include <cc/testing>

int main(int argc, char *argv[])
{
    using namespace cc;

    TestCase {
        "LocalEchoTest",
        []{
            String dirPath = Dir::createTemp();
            CC_VERIFY(Dir::exists(dirPath));

            ScopeGuard cleanup {[=]{
                File::clean(dirPath);
            }};

            SocketAddress address{dirPath / "echo"};
            CC_INSPECT(address);

            Semaphore<int> go;

            Thread server{[&]{
                ServerSocket socket{address};
                go.release();
                StreamSocket client = socket.accept();
                client.transferTo(client);
            }};

            Thread client{[&]{
                ClientSocket socket{address};
                String message = "Hello, echo!";
                socket.write(message);
                String echo = socket.readSpan(message.count());
                CC_CHECK_EQUALS(message, echo);
            }};

            server.start();
            go.acquire();
            client.start();
            server.wait();
            client.wait();
        }
    };

    return TestSuite{argc, argv}.run();
}
