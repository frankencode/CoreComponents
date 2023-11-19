#include <cc/HttpClient>
#include <cc/IoStream>
#include <cc/File>
#include <cc/exceptions>
#include <cc/stdio>

using namespace cc;

int query(const String &method, const String &location)
{
    Uri uri{location};
    HttpClient client{uri};
    if (!client.waitEstablished()) return 2;

    ferr() << "Connected to " << client.address() << nl;

    HttpClient::Generate generate;

    if (method == "PUT" || method == "POST") {
        generate = [](HttpMessageGenerator &generator) {
            IoStream::input().transferTo(generator.payload());
        };
    }

    HttpResponse response = client.query(method, uri.requestPath(), generate);
    ferr() << response.version() << " " << +response.status() << " " << response.reasonPhrase() << nl;

    response.payload().transferTo(IoStream::output());

    return +response.status() >= 300 ? +response.status() / 100 : 0;
}

int main(int argc, char *argv[])
{
    auto toolName = String{argv[0]}.baseName();

    try {
        if (argc == 2) {
            return query("GET", argv[1]);
        }
        else if (argc == 3) {
            return query(String{argv[1]}.upcased(), argv[2]);
        }
        else {
            throw HelpRequest{};
        }
    }
    catch (HelpRequest &) {
        ferr(
            "Usage: %% [GET|PUT|...] <URI>\n"
            "Simple HTTP(S) client\n"
            "\n"
        ) << toolName;
        return 1;
    }
    catch (Exception &other) {
        ferr() << other << nl;
        return 2;
    }

    return 0;
}
