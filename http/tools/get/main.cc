#include <cc/http/HttpClientSocket>
#include <cc/http/HttpClientConnection>
#include <cc/http/HttpRequestGenerator>
#include <cc/net/Uri>
#include <cc/stdio>
#include <cc/KernelInfo>

using namespace cc;
using namespace cc::net;
using namespace cc::http;

int main(int argc, char **argv)
{
    auto toolName = String{argv[0]}->fileName();

    try {
        if (argc < 2) throw HelpRequest{};

        String commandLine;
        {
            auto parts = StringList::create();
            for (int i = 1; i < argc; ++i)
                parts->append(argv[i]);
            commandLine = parts->join("%20");
            commandLine = commandLine->replace(" ", "%20");
        }

        auto security = HttpClientSecurity::createDefault();
        auto uri = Uri::parse(commandLine);
        if (uri->port() <= 0) uri->setPort(uri->scheme() == "https" ? 443 : 80);
        if (uri->path() == "") uri->setPath("/");
        if (uri->scheme() == "http" || uri->scheme() == "local") security = nullptr;
        auto address = SocketAddress::resolve(uri);
        auto socket = HttpClientSocket::connect(address, uri->host(), security);
        auto connection = HttpClientConnection::open(socket);
        auto request = HttpRequestGenerator::create(connection);
        request->setMethod("GET");
        request->setHost(uri->requestHost());
        request->setPath(uri->requestPath());
        {
            auto kernelInfo = KernelInfo::query();
            request->setHeader("User-Agent", Format{"ccnode 1.0 (%%; %%)"} << kernelInfo->name() << kernelInfo->machine());
        }
        request->transmit();
        auto response = connection->readResponse();
        if (response->statusCode() == 200) {
            response->payload()->transferTo(stdOut());
        }
        else {
            ferr() << response->version() << " " << response->statusCode() << " " << response->reasonPhrase() << nl;
        }
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% <URI>\n"
            "Simple HTTP(S) client\n"
            "\n"
        ) << toolName;
    }

    return 0;
}
