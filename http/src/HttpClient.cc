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

Ref<HttpResponse> HttpClient::query(const String &method, const Uri &uri, const String &payload)
{
    return HttpClient::connect(uri)->query(method, uri->path(), payload);
}

Ref<HttpResponse> HttpClient::query(const String &method, const Uri &uri, Stream *source)
{
    return HttpClient::connect(uri)->query(method, uri->path(), source);
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
    if (address_->port() <= 0) address_->setPort(uri->scheme() == "https" ? 443 : 80);
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

Ref<HttpResponse> HttpClient::query(const String &method, const String &path, const std::function<void(HttpRequestGenerator *)> &sendPayload)
{
    Ref<HttpResponse> response;
    for (int i = 0; retry(i); ++i) {
        try {
            auto request = createRequest(method, path);
            sendPayload(request);
            response = connection_->readResponse();
            if (response->statusCode() != RequestTimeout::StatusCode) break;
        }
        catch (CloseRequest &)
        {}
        catch (ConnectionResetByPeer &)
        {}
        connect();
    }
    if (response->statusCode() >= 400) throw HttpError{response->statusCode()};
    return response;
}

Ref<HttpResponse> HttpClient::query(const String &method, const String &path, const String &payload)
{
    return query(method, path,
        [=](HttpRequestGenerator *request) {
            request->beginTransmission(payload->count());
            if (payload->count() > 0)
                request->write(payload);
            request->endTransmission();
        }
    );
}

Ref<HttpResponse> HttpClient::query(const String &method, const String &path, Stream *source)
{
    return query(method, path,
        [=](HttpRequestGenerator *request) {
            request->beginTransmission(source ? -1 : 0);
            if (source) source->transferTo(request->payload());
            request->endTransmission();
        }
    );
}

String HttpClient::userAgent() const
{
    static String s =
        Format{"cchttp 1.0 (%%; %%)"}
            << KernelInfo::instance()->name()
            << KernelInfo::instance()->machine();

    return s;
}

}} // namespace cc::http
