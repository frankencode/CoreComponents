#include <cc/http/HttpClient>
#include <cc/stdio>

using namespace cc;
using namespace cc::net;
using namespace cc::http;

int main(int argc, char **argv)
{
    auto toolName = String{argv[0]}->fileName();
    int exitStatus = 0;

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

        auto response = HttpClient::get(commandLine);

        if (response->statusCode() >= 300) exitStatus = response->statusCode() / 100;

        ferr() << response->version() << " " << response->statusCode() << " " << response->reasonPhrase() << nl;
        response->payload()->transferTo(stdOut());
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% <URI>\n"
            "Simple HTTP(S) client\n"
            "\n"
        ) << toolName;
    }

    return exitStatus;
}
