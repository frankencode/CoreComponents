#include <cc/HttpClient>
#include <cc/IoStream>
#include <cc/File>
#include <cc/exceptions>
#include <cc/stdio>
#include <cc/DEBUG>

using namespace cc;

int get(const String &location, const String &fileName)
{
    Uri uri{location};
    HttpClient client{uri};
    if (!client.waitEstablished()) return 2;

    ferr() << "Connected to " << client.address() << nl;

    HttpResponse response = client.query("GET", uri.requestPath());
    ferr() << response.version() << " " << +response.status() << " " << response.reasonPhrase() << nl;
    if (
        response.status() == HttpStatus::MovedTemporarily ||
        response.status() == HttpStatus::SeeOther ||
        response.status() == HttpStatus::TemporaryRedirect
    ) {
        String newLocation = response.header("Location");
        ferr() << "Location: " << newLocation << nl;

        response.payload().transferTo(IoStream::error());

        return get(newLocation, fileName);
    }

    File file{fileName, FileOpen::Overwrite};
    response.payload().transferTo(file);

    return +response.status() >= 300 ? +response.status() / 100 : 0;
}

int main(int argc, char *argv[])
{
    auto toolName = String{argv[0]}.baseName();

    try {
        if (argc != 2) throw HelpRequest{};
        String location = argv[1];
        String fileName = location.fileName();
        return get(location, fileName);
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
