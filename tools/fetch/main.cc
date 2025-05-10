#include <cc/HttpClient>
#include <cc/IoStream>
#include <cc/File>
#include <cc/CryptoHash>
#include <cc/StreamMultiplexer>
#include <cc/exceptions>
#include <cc/stdio>

using namespace cc;

int fetch(const String &location, const String &fileName, const CryptoHash &hash)
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

        response.payload().transferTo(stdError());

        return fetch(newLocation, fileName, hash);
    }

    File file{fileName, FileOpen::Overwrite};
    Stream sink = file;

    if (hash.isValid()) {
        CryptoHashSink hashSink = hash.openSink();
        response.payload().transferTo(
            StreamMultiplexer{file, hashSink}
        );
        Bytes sum = hashSink.finish();
        if (sum != hash.sum()) {
            ferr() << "Invalid hash sum, got: " << hex(sum) << ", expected: " << hex(hash.sum()) << nl;
            return 2;
        }
    }
    else {
        response.payload().transferTo(file);
    }

    return +response.status() >= 300 ? +response.status() / 100 : 0;
}

int main(int argc, char *argv[])
{
    auto toolName = String{argv[0]}.baseName();

    try {
        String location;
        CryptoHash hash;
        if (argc == 2) {
            location = argv[1];
        }
        else if (argc == 3) {
            location = argv[1];
            hash = CryptoHash{argv[2]};
            if (!hash.isValid()) throw HelpRequest{};
        }
        else throw HelpRequest{};

        return fetch(location, location.fileName(), hash);
    }
    catch (HelpRequest &) {
        ferr(
            "Usage: %% <URI> [MD5|SHA1|SHA256|SHA512|...:<HASH>]\n"
            "Fetch file from a web server (and verify checksum on-the-fly)\n"
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
