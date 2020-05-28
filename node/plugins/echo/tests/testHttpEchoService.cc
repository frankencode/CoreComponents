/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/testing/TestSuite>
#include <cc/http/NodeMaster>
#include <cc/http/HttpClientSocket>
#include <cc/http/HttpClientConnection>
#include <cc/http/HttpRequestGenerator>
#include <cc/stdio>

using namespace cc;
using namespace cc::testing;
using namespace cc::net;
using namespace cc::http;

class SimpleEchoTest: public TestCase
{
    void run() override
    {
        auto server = NodeMaster::create(
            "Node {\n"
            "    address: \"127.0.0.1\"\n"
            "    port: 0\n"
            "    family: IPv4\n"
            "    concurrency: 1\n"
            "    Echo {\n"
            "        host: *\n"
            "    }\n"
            "}\n"
        );
        server->start();
        auto address = server->waitStarted();
        auto socket = HttpClientSocket::connect(address, "127.0.0.1");
        auto connection = HttpClientConnection::open(socket);
        auto request = HttpRequestGenerator::create(connection);
        request->setMethod("GET");
        request->setHost("localhost");
        request->setPath("/");
        request->transmit();
        auto response = connection->readResponse();
        CC_VERIFY(response->statusCode() == 200);
        String text = response->payload()->readAll();
        fout() << text;
        CC_VERIFY(text->contains("localhost"));
        CC_VERIFY(text->startsWith("GET"));
        server->sendSignal(Signal::Terminate);
        server->wait();
    }
};

int main(int argc, char** argv)
{
    CC_TESTSUITE_ADD(SimpleEchoTest);

    return TestSuite::instance()->run(argc, argv);
}
