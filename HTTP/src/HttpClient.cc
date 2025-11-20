/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpClient>
#include <cc/HttpResponseParser>
#include <cc/TlsClientStream>
#include <cc/HttpStream>
#include <cc/TapBuffer>
#include <cc/ClientSocket>
#include <cc/Uri>
#include <cc/StreamTap>
#include <cc/KernelInfo>

namespace cc {

struct HttpClient::State: public Object::State
{
    State(const Uri &uri, const TlsClientOptions &tlsOptions):
        address_{uri.resolve()},
        host_{uri.host()}
    {
        const bool useTls = !(
            uri.scheme() == "http" ||
            (uri.scheme() == "" && (uri.port() == 80 || uri.port() == 8080))
        );
        if (uri.port() <= 0) {
            address_.setPort(useTls ? 443 : 80);
        }
        initiate(useTls, tlsOptions);
    }

    State(const SocketAddress &address, const TlsClientOptions &tlsOptions):
        address_{address},
        host_{address.networkAddress()}
    {
        const bool useTls = !(address.port() == 80 || address.port() == 8080);
        initiate(useTls, tlsOptions);
    }

    ~State()
    {
        try {
            parser_ = HttpResponseParser{};
            stream_ = Stream{};
            socket_.shutdown();
        }
        catch (...)
        {}
    }

    void initiate(bool useTls, TlsClientOptions tlsOptions)
    {
        socket_ = ClientSocket{address_};
        stream_ = socket_;
        if (useTls) {
            TlsClientOptions options = tlsOptions;
            if (options.serverName() == "") options.setServerName(host_);
            stream_ = TlsClientStream{stream_, options};
        }
        parser_ = HttpResponseParser{stream_};
    }

    HttpResponse query(const String &method, const String &path, const Generate &generate)
    {
        HttpRequestGenerator request{stream_};
        request.setMethod(method);
        request.setHost(host_);
        request.setPath(path != "" ? path : "/");
        request.setHeader("User-Agent", userAgent());
        generate(request);
        return parser_.readResponse();
    }

    static String userAgent()
    {
        KernelInfo kernel;

        static String info =
            Format{"cc_http 1.0 (%%; %%)"}
                << kernel.name()
                << kernel.machine();

        return info;
    }

    static void generateDefault(HttpMessageGenerator &request)
    {
        request.transmit();
    }

    SocketAddress address_;
    String host_;
    ClientSocket socket_;
    Stream stream_;
    HttpResponseParser parser_;
};

HttpClient::HttpClient(const Uri &uri, const TlsClientOptions &tlsOptions):
    Object{new State{uri, tlsOptions}}
{}

HttpClient::HttpClient(const SocketAddress &address, const TlsClientOptions &tlsOptions):
    Object{new State{address, tlsOptions}}
{}

SocketAddress HttpClient::address() const
{
    return me().address_;
}

bool HttpClient::waitEstablished(int timeout)
{
    return me().stream_.waitEstablished(timeout);
}

HttpResponse HttpClient::query(const String &method, const String &path, const Generate &generate)
{
    return me().query(method, path, generate ? generate : HttpClient::State::generateDefault);
}

HttpClient::State &HttpClient::me()
{
    return Object::me.as<State>();
}

const HttpClient::State &HttpClient::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
