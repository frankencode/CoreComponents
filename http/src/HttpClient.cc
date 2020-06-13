/*
 * Copyright (C) 2020 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/HttpClient>
#include <cc/http/HttpClientSocket>
#include <cc/http/HttpClientConnection>
#include <cc/http/HttpRequestGenerator>
#include <cc/KernelInfo>
#include <cc/Format>

namespace cc {
namespace http {

Ref<HttpResponse> HttpClient::get(const Uri &uri)
{
    return HttpClient::connect(uri)->query("GET", uri->requestPath());
}

Ref<HttpResponse> HttpClient::head(const Uri &uri)
{
    return HttpClient::connect(uri)->query("HEAD", uri->requestPath());
}

Ref<HttpResponse> HttpClient::put(const Uri &uri, const String &payload)
{
    return HttpClient::connect(uri)->query("PUT", uri->requestPath(),
        [=](HttpGenerator *request) { request->transmit(payload); }
    );
}

Ref<HttpResponse> HttpClient::put(const Uri &uri, Stream *source)
{
    return HttpClient::connect(uri)->query("PUT", uri->requestPath(),
        [=](HttpGenerator *request) { request->transmit(source); }
    );
}

Ref<HttpResponse> HttpClient::post(const Uri &uri, const String &payload)
{
    return HttpClient::connect(uri)->query("POST", uri->requestPath(),
        [=](HttpGenerator *request) { request->transmit(payload); }
    );
}

Ref<HttpResponse> HttpClient::post(const Uri &uri, Stream *source)
{
    return HttpClient::connect(uri)->query("POST", uri->requestPath(),
        [=](HttpGenerator *request) { request->transmit(source); }
    );
}

Ref<HttpResponse> HttpClient::postForm(const Uri &uri, const MapValue<String> &form)
{
    return HttpClient::connect(uri)->query("POST", uri->requestPath(),
        [=](HttpGenerator *request) {
            request->setHeader("Content-Type", "application/x-www-form-urlencoded");
            request->transmit(Uri::encodeForm(form));
        }
    );
}

Ref<HttpClient> HttpClient::connect(const Uri &uri, const HttpClientSecurity *security)
{
    return new HttpClient{uri, security};
}

HttpClient::HttpClient(const Uri &uri, const HttpClientSecurity *security):
    address_{SocketAddress::resolveUri(uri)},
    host_{uri->host()},
    security_{security}
{
    if (uri->port() <= 0) address_->setPort(uri->scheme() == "https" ? 443 : 80);
    if (!security_ && uri->scheme() == "https") security_ = HttpClientSecurity::createDefault();
    connect();
}

void HttpClient::connect()
{
    connection_ = HttpClientConnection::open(
        HttpClientSocket::connect(address_, host_, security_)
    );
}

Ref<HttpRequestGenerator> HttpClient::createRequest(const String &method, const String &path)
{
    auto request = HttpRequestGenerator::create(connection_);
    request->setMethod(method);
    request->setHost(host_);
    request->setPath(path != "" ? path : "/");
    {
        String h = userAgent();
        if (h != "") request->setHeader("User-Agent", h);
    }
    return request;
}

Ref<HttpResponse> HttpClient::query(const String &method, const String &path, const Generate &generate)
{
    Ref<HttpResponse> response;
    for (int i = 0; retry(i); ++i) {
        try {
            generate(createRequest(method, path));
            response = connection_->readResponse();
            if (response->statusCode() != RequestTimeout::StatusCode) break;
        }
        catch (CloseRequest &)
        {}
        catch (ConnectionResetByPeer &)
        {}
        connect();
    }
    return response;
}

void HttpClient::pipeline(const String &method, const String &path, const Generate &generate)
{
    generate(createRequest(method, path));
}

Ref<HttpResponse> HttpClient::readResponse()
{
    return connection_->readResponse();
}

String HttpClient::userAgent() const
{
    KernelInfo kernel;

    static String s =
        Format{"cchttp 1.0 (%%; %%)"}
            << kernel->name()
            << kernel->machine();

    return s;
}

bool HttpClient::retry(int i)
{
    return i < 3;
}

void HttpClient::generateDefault(HttpGenerator *request)
{
    request->transmit();
}

}} // namespace cc::http
