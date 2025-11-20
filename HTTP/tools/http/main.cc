#include <cc/HttpClient>
#include <cc/Arguments>
#include <cc/IoStream>
#include <cc/File>
#include <cc/exceptions>
#include <cc/stdio>

using namespace cc;

int query(const Map<String, Variant> &options, const String &location)
{
    String method = options("method");

    Uri uri{location};
    TlsClientOptions tlsOptions;

    if (options("ca") != "") {
        tlsOptions.setTrustFilePath(options("ca"));
    }
    if (options("cn") != "") {
        tlsOptions.setServerName(options("cn"));
    }
    if (options("cert") != "") {
        if (options("key") == "") throw HelpRequest{};
        tlsOptions.setCredentials(options("cert"), options("key"));
    }
    if (options("psk") != "") {
        String psk = File{options("psk")}.readAll();
        String pskId;
        if (options("psk-id") != "") pskId = File{options("psk-id")}.readAll();
        tlsOptions.setPsk(psk, pskId);
    }

    HttpClient client{uri, tlsOptions};
    if (!client.waitEstablished()) return 2;

    ferr() << "Connected to " << client.address() << nl;

    HttpClient::Generate generate;

    if (method == "PUT" || method == "POST") {
        generate = [](HttpMessageGenerator &generator) {
            stdInput().transferTo(generator.payload());
        };
    }

    HttpResponse response = client.query(method, uri.requestPath(), generate);
    ferr() << response.version() << " " << +response.status() << " " << response.reasonPhrase() << nl;

    response.payload().transferTo(stdOutput());

    return +response.status() >= 300 ? +response.status() / 100 : 0;
}

int main(int argc, char *argv[])
{
    auto toolName = String{argv[0]}.baseName();

    try {
        Map<String, Variant> options;
        options.insert("method", "GET");
        options.insert("ca", "");
        options.insert("cn", "");
        options.insert("cert", "");
        options.insert("key", "");
        options.insert("psk", "");
        options.insert("psk-hex", "");
        options.insert("psk-id", "");
        options.insert("psk-id-hex", "");

        List<String> items = Arguments{argc, argv}.read(&options);
        if (items.count() != 1) throw HelpRequest{};

        return query(options, items(0));
    }
    catch (HelpRequest &) {
        ferr(
            "Usage: %% [OPTION]... <URI>\n"
            "Simple HTTP(S) client\n"
            "\n"
            "Options:\n"
            "  -method  query method (\"GET\", \"PUT\", \"POST\", \"HEAD\", etc.)\n"
            "  -ca      server trust anchor(s) file (PEM format)\n"
            "  -cn      expected server name (used for SNI and certificate CN check)\n"
            "  -cert    client certificate file (PEM format)\n"
            "  -key     client key file (PEM format)\n"
            "  -psk     pre-shared key file\n"
            "  -psk-id  pre-shared key identifier file (optional)\n"
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
