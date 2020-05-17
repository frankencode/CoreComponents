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

Ref<HttpClient> HttpClient::connect(const Uri &uri, const HttpClientSecurity *security)
{
    return new HttpClient{uri, security};
}

HttpClient::HttpClient(const Uri &uri, const HttpClientSecurity *security):
    host_{uri->host()}
{
    auto address = SocketAddress::resolveUri(uri);
    if (address->port() <= 0) address->setPort(uri->scheme() == "https" ? 443 : 80);

    connection_ = HttpClientConnection::open(
        HttpClientSocket::connect(address, uri->host(), security)
    );
}

String HttpClient::userAgent() const
{
    static String s =
        Format{"ccget 1.0 (%%; %%)"}
            << KernelInfo::instance()->name()
            << KernelInfo::instance()->machine();
    return s;
}

Ref<HttpResponse> HttpClient::query(const String &method, const String &path, const String &payload)
{
    auto request = HttpRequestGenerator::create(connection_);
    request->setMethod(method);
    request->setHost(host_);
    request->setPath(path);
    {
        String h = userAgent();
        if (h != "") request->setHeader("User-Agent", h);
    }
    request->beginTransmission(payload->count());
    request->write(payload);
    request->endTransmission();
    return connection_->readResponse();
}

Ref<HttpResponse> HttpClient::get(const String &path)
{
    return query("GET", path);
}

Ref<HttpResponse> HttpClient::head(const String &path)
{
    return query("HEAD", path);
}

Ref<HttpResponse> HttpClient::put(const String &path, const String &payload)
{
    return query("PUT", path, payload);
}

Ref<HttpResponse> HttpClient::post(const String &path, const String &payload)
{
    return query("POST", path, payload);
}

// Ref<HttpResponse> HttpClient::postForm(const String &path, const Map<String> *inputs) { return query("POST", path, urlEncode(inputs)); }

}} // namespace cc::http
