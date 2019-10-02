/*
 * Copyright (C) 2007-2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/node/NodeConfigProtocol>
#include <cc/node/DeliveryRegistry>
#include <cc/node/NodeMaster>
#include <cc/exceptions>
#include <cc/stdio>
#include <cc/SignalMaster>
#include <cc/Process>
#include <cc/File>
#include <cc/Dir>
#include <cc/ResourceGuard>
#include <cc/Arguments>

using namespace cc;
using namespace cc::meta;
using namespace cc::node;

Ref<NodeConfig> loadNodeConfig(int argc, char **argv)
{
    Ref<Arguments> arguments = Arguments::parse(argc, argv);
    const StringList *items = arguments->items();

    arguments->validate(NodeConfig::prototype());

    Ref<MetaObject> config;
    String dirPath;

    if (items->count() > 0) {
        if (items->count() > 1)
            throw UsageError{"Handling multiple input arguments at once is not supported"};

        String path = items->at(0);
        if (Dir::exists(path)) {
            dirPath = path;
        }
        else {
            ResourceGuard context{path};
            config = NodeConfig::parse(File::open(path)->map());
        }
    }

    if (!config) config = NodeConfig::prototype()->clone();
    arguments->override(config);

    Ref<NodeConfig> nodeConfig = NodeConfig::load(config);
    if (dirPath != "") nodeConfig->addDirectoryInstance(dirPath);
    if (nodeConfig->deliveryInstances()->count() == 0) nodeConfig->addEchoInstance();

    return nodeConfig;
}

int main(int argc, char **argv)
{
    String toolName = String{argv[0]}->fileName();
    int exitCode = 0;

    try {
        Thread::blockSignals(SignalSet::createFull());

        auto config = loadNodeConfig(argc, argv);

        if (config->daemon() && !Process::isDaemonized())
            Process::daemonize();

        auto node = NodeMaster::create(config);

        auto signalMaster = SignalMaster::start([=](Signal signal, bool *fin){
            node->signaled(signal);
            *fin = (+signal == SIGINT || +signal == SIGTERM);
        });

        node->start();
        node->wait();

        signalMaster->wait();

        exitCode = node->exitCode();
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]... [CONFIG]|[DIR]...\n"
            "Start a HTTP server on this machine.\n"
            "\n"
            "Options:\n"
            "  -address   host name or network address to listen on\n"
            "  -port      port to listen on (default 8080)\n"
            "  -protocol  protocol: IPv4 or IPv6 (default both)\n"
            "  -user      switch to user after opening listening socket\n"
            "  -daemon    start as a daemon\n"
        ) << toolName;
    }

    return exitCode;
}
