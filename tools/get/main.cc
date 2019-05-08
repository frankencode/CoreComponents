#include <cc/stdio>
#include <cc/Arguments>
#include <cc/net/Uri>
#include <cc/http/HttpClientSocket>
#include <cc/http/HttpClientConnection>
#include <cc/http/HttpRequestGenerator>

using namespace cc;
using namespace cc::net;
using namespace cc::http;

int main(int argc, char **argv)
{
    String toolName = String{argv[0]}->fileName();

    try {
        auto options = VariantMap::create();
        auto arguments = Arguments::parse(argc, argv, options);
        auto items = arguments->items();

        for (auto item: items) {
            auto uri = Uri::parse(item);
            if (uri->port() <= 0) uri->setPort(uri->scheme() == "https" ? 443 : 80);
            if (uri->path() == "") uri->setPath("/");
            auto address = SocketAddress::resolve(uri);
            auto socket = HttpClientSocket::connect(address, uri->host());
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
