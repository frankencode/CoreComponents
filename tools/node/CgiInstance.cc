/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/net/Uri>
#include <cc/net/SocketAddress>
#include <cc/http/exceptions>
#include "ErrorLog.h"
#include "CgiInstance.h"

namespace ccnode {

using namespace cc::http;

Ref<CgiInstance> CgiInstance::create(MetaObject *config)
{
    return new CgiInstance{config};
}

CgiInstance::CgiInstance(MetaObject *config):
    ServiceInstance{config},
    script_{config->value("script")},
    server_{config->value("server")},
    environment_{config->value("environment")},
    serverAddress_{SocketAddressList::create()},
    delegateCount_{0}
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
        catch (SyntaxError &ex) {
            throw UsageError{Format{"CGI: Invalid address syntax \"%%\""} << server_->at(i)};
        }
    }
}

Ref<ServiceDelegate> CgiInstance::createDelegate(ServiceWorker *worker) const
{
    return CgiDelegate::create(worker);
}

} // namespace ccnode
