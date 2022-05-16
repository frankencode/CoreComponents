#include <cc/HttpClient>
#include <cc/IoStream>
#include <cc/exceptions>
#include <cc/stdio>

int main(int argc, char *argv[])
{
    using namespace cc;

    auto toolName = String{argv[0]}.baseName();

    try {
        if (argc != 2) throw HelpRequest{};

        Uri uri{argv[1]};
        HttpClient client{uri};
        if (!client.waitEstablished()) return 2;

        ferr() << "Connected to " << client.address() << nl;

        HttpResponse response = client.query("GET", uri.path());
        ferr() << response.version() << " " << +response.status() << " " << response.reasonPhrase() << nl;
        response.payload().transferTo(IoStream::output());
        // IoStream::error().write("\n");
        return +response.status() >= 300 ? +response.status() / 100 : 0;
    }
    catch (HelpRequest &) {
        ferr(
            "Usage: %% <URI>\n"
            "Simple HTTP(S) client\n"
            "\n"
        ) << toolName;
        return 1;
    }
    #ifdef NDEBUG
    catch (Exception &other) {
        ferr() << other << nl;
        return 2;
    }
    #endif

    return 0;
}
