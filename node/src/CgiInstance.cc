/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/CgiInstance>
#include <cc/node/ErrorLog>
#include <cc/http/exceptions>
#include <cc/net/Uri>
#include <cc/net/SocketAddress>

namespace cc {
namespace node {

using namespace cc::http;

Ref<CgiInstance> CgiInstance::create(const MetaObject *config)
{
    return new CgiInstance{config};
}

CgiInstance::CgiInstance(const MetaObject *config):
    DeliveryInstance{config},
    script_{config->value("script")},
    server_{config->value("server")},
    environment_{config->value("environment")},
    serverAddress_{SocketAddressList::create()}
{
    /*if (script_ == "" && server_->count() == 0)
        throw UsageError{"CGI: Please provide a \"script\" or a \"server\""};*/

    for (int i = 0; i < server_->count(); ++i) {
        try {
            Ref<SocketAddress> address;
            Ref<Uri> uri = Uri::parse(server_->at(i));
            if (uri->hostIsNumeric()) {
                address = SocketAddress::create(ProtocolFamily::Unspecified, uri->host(), uri->port());
            }
            else {
                CCNODE_NOTICE() << "Resolving host name of server \"" << uri->host() << "\"..." << nl;
                Ref<SocketAddressList> list = SocketAddress::resolve(uri->host());
                if (list->count() == 0)
                    throw UsageError{Format{"CGI: Failed to resolve host name of server \"%%\""} << uri->host()};
                address = list->at(0);
                address->setPort(uri->port());
            }
            serverAddress_->append(address);
            CCNODE_NOTICE() << "  " << address << nl;
        }
        catch (TextError &ex) {
            throw UsageError{Format{"CGI: Invalid address syntax \"%%\""} << server_->at(i)};
        }
    }
}

Ref<DeliveryDelegate> CgiInstance::createDelegate(DeliveryWorker *worker) const
{
    return CgiDelegate::create(worker);
}

}} // namespace cc::node
