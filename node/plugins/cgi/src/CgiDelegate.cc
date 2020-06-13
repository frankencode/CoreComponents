/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/http/CgiDelegate>
#include <cc/http/NodeConfig>
#include <cc/http/MediaTypeDatabase>
#include <cc/http/DeliveryWorker>
#include <cc/http/CgiServerConnection>
#include <cc/http/CgiInstance>
#include <cc/http/exceptions>
#include <cc/http/debug>
#include <cc/http/TapBuffer>
#include <cc/net/StreamSocket>
#include <cc/str>
#include <cc/Format>
#include <cc/File>
#include <cc/Process>
#include <cc/SocketPair>
#include <cc/StreamTap>

namespace cc {
namespace http {

Ref<CgiDelegate> CgiDelegate::create(DeliveryWorker *worker)
{
    return new CgiDelegate{worker};
}

CgiDelegate::CgiDelegate(DeliveryWorker *worker):
    DeliveryDelegate{worker},
    cgiInstance_{worker->deliveryInstance()},
    nextPeer_{cgiInstance_->randomSeed()}
{}

void CgiDelegate::process(const HttpRequest *request)
{
    process(request, cgiInstance_->script());
}

bool CgiDelegate::process(const HttpRequest *request, const FileStatus *status, const String &documentRoot)
{
    String script;

    if ((status->type() == FileType::Regular) && +(status->mode() & FileMode::AnyExec))
        script = status->path();
    else
        script = cgiInstance_->script();

    process(request, script, documentRoot);
    return true;
}

void CgiDelegate::process(const HttpRequest *request, const String &script, const String &documentRoot)
{
    Ref<CgiServerConnection> cgiServer;
    Process sub;

    String payload;
    {
        int payloadSize = 0; {
            String h;
            if (request->lookup("Content-Length", &h))
                payloadSize = h->toNumber<int>();
        }

        if (payloadSize > 0) {
            if (payloadSize > cgiInstance_->requestPayloadLimit())
                throw PayloadTooLarge{};
            payload = request->payload()->readSpan(payloadSize);
        }
    }

    if (cgiInstance_->serverAddress()->count() > 0)
    {
        int nextIndex = (nextPeer_++) % cgiInstance_->serverAddress()->count();
        SocketAddress address = cgiInstance_->serverAddress()->at(nextIndex);
        if (cgiInstance_->serverAddress()->count() > 1)
            CCNODE_DEBUG() << "Forwarding request to server " << address << nl;

        cgiServer = CgiServerConnection::open(StreamSocket::connect(address));
        if (errorLoggingInstance()->verbosity() >= LoggingLevel::Debug)
            cgiServer->setupTransferLog(errorLoggingInstance()->debugStream(), address->toString());

        String headerText = compileHeader(request, mutate(payload));
        cgiServer->stream()->write(
            Format{}
                << headerText->count() << ":"
                << headerText << "," << payload
        );
    }
    else if (script != "")
    {
        StringList args = script->split(' ');
        String scriptPath = args->at(0);
        if (!scriptPath->isAbsolutePath()) scriptPath = documentRoot->extendPath(scriptPath);
        args->at(0) = scriptPath;

        EnvMap env = makeEnv(request, mutate(payload));
        env->insert("SCRIPT_NAME", "/" + scriptPath->baseName());
        if (documentRoot != "") env->insert("DOCUMENT_ROOT", documentRoot);

        Ref<SocketPair> pair = SocketPair::create();
        sub =
            Process{
                Command{}
                ->setArgs(args)
                ->setEnvMap(env)
                ->setInputChannel(pair)
                ->setOutputChannel(pair)
                ->setErrorChannel(pair)
            };

        cgiServer = CgiServerConnection::open(pair);
        if (errorLoggingInstance()->verbosity() >= LoggingLevel::Debug)
            cgiServer->setupTransferLog(errorLoggingInstance()->debugStream(), scriptPath->baseName());

        if (payload->count() > 0)
            cgiServer->stream()->write(payload);

        sub->input()->close();
    }
    else
        throw InternalServerError{};

    Ref<HttpMessage> cgiResponse = cgiServer->readResponse();

    int statusCode = -1;
    String reasonPhrase;
    {
        HttpMessage::iterator target;
        if (cgiResponse->find("Status", &target)) {
            String statusField = target->value();
            int i = statusField->scan(' ');
            statusCode = statusField->select(0, i)->toNumber<int>();
            if (i < statusField->count())
                reasonPhrase = statusField->copy(i + 1, statusField->count());
            if (statusCode == 0) {
                CCNODE_ERROR() << "Mailformed Status field in CGI response: \"" << statusField << "\"" << nl;
                throw InternalServerError{};
            }
            cgiResponse->remove(target);
        }
    }

    {
        HttpMessage::iterator target;
        if (cgiResponse->find("Location", &target)) {
            String location = target->value();
            CCNODE_DEBUG() << "Redirect to \"" << location << "\"" << nl;
            if (location->startsWith("/")) {
                cgiResponse->remove(target);
                String content = File::open(location)->map();
                String contentType;
                if (!cgiResponse->lookup("Content-Type", &contentType)) {
                    contentType = deliveryInstance()->mediaTypeDatabase()->lookup(location, content);
                    cgiResponse->insert("Content-Type", contentType);
                }
                response()->setStatus(statusCode, reasonPhrase);
                for (int i = 0; i < cgiResponse->count(); ++i)
                    response()->setHeader(cgiResponse->at(i)->key(), cgiResponse->at(i)->value());
                response()->beginTransmission(content->count());
                response()->write(content);
                response()->endTransmission();
                return;
            }
            else if (statusCode < 0) {
                statusCode = 302;
                reasonPhrase = "Found";
            }
        }
    }

    if (statusCode < 0) {
        statusCode = 200;
        reasonPhrase = "OK";
    }

    response()->setStatus(statusCode, reasonPhrase);
    ssize_t contentLength = -1;
    {
        HttpMessage::iterator target;
        if (cgiResponse->find("Content-Length", &target)) {
            contentLength = target->value()->toNumber<size_t>();
            cgiResponse->remove(target);
        }
    }
    response()->setHeader(cgiResponse);
    autoSecureForwardings();

    if (request->majorVersion() == 1 && request->minorVersion() == 0)
    {
        String content = cgiResponse->payload()->readSpan(contentLength);
        response()->beginTransmission(content->count());
        response()->write(content);
        response()->endTransmission();
    }
    else {
        response()->beginTransmission();
        ssize_t totalTransferred = 0;
        {
            String buffer = String::allocate(0x10000);
            while (true) {
                int n = cgiResponse->payload()->read(mutate(buffer));
                if (n == 0) break;
                response()->write(buffer->select(0, n));
                totalTransferred += n;
                if (contentLength >= 0) {
                    if (totalTransferred >= contentLength) break;
                }
            }
            response()->endTransmission();
        }
    }

    if (sub) {
        int ret = sub->wait();
        if (ret != 0)
            CCNODE_ERROR() << "CGI process return error, ret = " << ret << nl;
    }
}

EnvMap CgiDelegate::makeEnv(const HttpRequest *request, CharArray *payload) const
{
    String queryString = urlDecode(request, payload);

    EnvMap env;
    env->insert("CONTENT_LENGTH", str(payload->count()));
    env->insert("REQUEST_METHOD", request->method());
    env->insert("REQUEST_URI", request->uri());
    env->insert("QUERY_STRING", queryString);
    env->insert("REMOTE_ADDR", client()->address()->networkAddress());
    env->insert("REMOTE_PORT", str(client()->address()->port()));
    env->insert("GATEWAY_INTERFACE", "CGI/1.1");
    env->insert("SERVER_PROTOCOL", request->version());
    env->insert("SERVER_NAME", request->host());
    env->insert("SERVER_SOFTWARE", nodeConfig()->version());

    {
        String h;
        if (request->lookup("Content-Type", &h))
            env->insert("CONTENT_TYPE", h);
    }

    if (client()->isSecure()) {
        env->insert("HTTP_X_FORWARDED_PROTO", "https");
        env->insert("HTTPS", "on");
    }

    for (const auto &pair: request)
        env->insert(wrapHttp(pair->key()), pair->value());

    for (const auto &pair: cgiInstance_->environment())
        env->insert(pair->key(), pair->value());

    return env;
}

void CgiDelegate::logEnv(const EnvMap &env)
{
    for (int i = 0; i < env->count(); ++i)
        CCNODE_DEBUG() << "environ[" << i << "] = \"" << env->at(i)->key() << "\": \"" << env->at(i)->value() << "\"" << nl;
}

String CgiDelegate::compileHeader(const HttpRequest *request, CharArray *payload) const
{
    String queryString = urlDecode(request, payload);

    Format header;
    header
        << "CONTENT_LENGTH" << payload->count()
        << "SCGI" << "1"
        << "REQUEST_METHOD" << request->method()
        << "REQUEST_URI" << request->uri()
        << "QUERY_STRING" << queryString
        << "REMOTE_ADDR" << client()->address()->networkAddress()
        << "REMOTE_PORT" << client()->address()->port()
        << "GATEWAY_INTERFACE" << "CGI/1.1"
        << "SERVER_PROTOCOL" << request->version()
        << "SERVER_NAME" << request->host()
        << "SERVER_SOFTWARE" << nodeConfig()->version();

    {
        String h;
        if (request->lookup("Content-Type", &h))
            header << "CONTENT_TYPE" << h;
    }

    if (client()->isSecure()) {
        header
            << "HTTP_X_FORWARDED_PROTO" << "https"
            << "HTTPS" << "on";
    }

    for (const auto &item: request)
        header << wrapHttp(item->key()) << item->value();

    VariantMap userEnv = cgiInstance_->environment();
    for (const auto &item: userEnv)
        header << item->key() << item->value();

    header << "";

    return String::join(header, String::create(1, '\0'));
}

String CgiDelegate::urlDecode(const HttpRequest *request, CharArray *payload)
{
    String queryString = request->query();
    if (request->method() == "POST") {
        if (request->value("Content-Type") == "application/x-www-form-urlencoded") {
            queryString = payload->copy(); // FIXME: not strict enough
            #if 0 // FIXME: needed?
            request->remove("Content-Type"); // FIXME: use removeAt or place contentType() into HttpRequest
            request->establish("Content-Length", "0");
            #endif
            payload->truncate(0);
        }
    }
    return queryString;
}

String CgiDelegate::wrapHttp(const String &header)
{
    String h = header->toUpper();
    mutate(h)->replaceInsitu('-','_');
    return str("HTTP_") + h;
}

}} // namespace cc::http
