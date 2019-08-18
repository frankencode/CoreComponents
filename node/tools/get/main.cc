#include <cc/node/HttpClientSocket>
#include <cc/node/HttpClientConnection>
#include <cc/node/HttpRequestGenerator>
#include <cc/net/Uri>
#include <cc/stdio>
#include <cc/Arguments>

using namespace cc;
using namespace cc::net;
using namespace cc::node;

int main(int argc, char **argv)
{
    auto toolName = String{argv[0]}->fileName();

    try {
        auto options = VariantMap::create();
        auto arguments = Arguments::parse(argc, argv, options);
        auto items = arguments->items();

        for (auto item: items) {
            auto security = HttpClientSecurity::createDefault();
            auto uri = Uri::parse(item);
            if (uri->port() <= 0) uri->setPort(uri->scheme() == "https" ? 443 : 80);
            if (uri->path() == "") uri->setPath("/");
            if (uri->scheme() == "http") security = nullptr;
            auto address = SocketAddress::resolve(uri);
            auto socket = HttpClientSocket::connect(address, uri->host(), security);
            auto connection = HttpClientConnection::open(socket);
            auto request = HttpRequestGenerator::create(connection);
            request->setMethod("GET");
            request->setHost(uri->requestHost());
            request->setPath(uri->requestPath());
            request->transmit();
            auto response = connection->readResponse();
            if (response->statusCode() == 200) {
                response->payload()->transferTo(stdOut());
            }
            else {
                ferr() << response->version() << " " << response->statusCode() << " " << response->reasonPhrase() << nl;
            }
        }
    }
    catch (HelpRequest &ex) {
        fout(
            "Usage: %% [URI]\n"
            "Simple HTTP client\n"
            "\n"
        ) << toolName;
    }

    return 0;
}
