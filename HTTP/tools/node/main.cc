/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/HttpServerConfigProtocol>
#include <cc/HttpServiceRegistry>
#include <cc/HttpServer>
#include <cc/SignalMaster>
#include <cc/Process>
#include <cc/File>
#include <cc/Dir>
#include <cc/ResourceGuard>
#include <cc/Thread>
#include <cc/Arguments>
#include <cc/exceptions>
#include <cc/stdio>

using namespace cc;

HttpServerConfig loadNodeConfig(int argc, char *argv[])
{
    Arguments arguments{argc, argv};
    List<String> items = arguments.items();

    arguments.validate(HttpServerConfig::prototype().members());

    MetaObject config;
    String dirPath;

    if (items.count() > 0) {
        if (items.count() > 1)
            throw UsageError{"Handling multiple input arguments at once is not supported"};

        String path = items.at(0);
        if (Dir::exists(path)) {
            dirPath = path;
        }
        else {
            ResourceGuard context{path};
            config = HttpServerConfig::parse(File{path}.map());
        }
    }

    if (!config) config = HttpServerConfig::prototype().clone();
    arguments.override(&config.members());

    HttpServerConfig nodeConfig{config};
    if (dirPath != "") nodeConfig.addDirectoryInstance(dirPath);
    if (nodeConfig.serviceInstances().count() == 0) nodeConfig.addEchoInstance();

    return nodeConfig;
}

int main(int argc, char *argv[])
{
    String toolName = String{argv[0]}.fileName();
    int exitCode = 0;

    try {
        Thread::blockSignals(SignalSet::full());

        auto config = loadNodeConfig(argc, argv);

        if (config.daemon() && !Process::isDaemonized())
            Process::daemonize();

        HttpServer server{config};

        SignalMaster signalMaster{server.signals()};
        signalMaster.start();

        server.start();
        server.wait();

        signalMaster.wait();

        exitCode = server.exitCode();
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]... [CONFIG]|[DIR]...\n"
            "Start a HTTP server on this machine.\n"
            "\n"
            "Options:\n"
            "  -address   host name or network address to listen on\n"
            "  -port      port to listen on (default: 8080)\n"
            "  -family    IPv4 or IPv6 (default: both)\n"
            "  -tls       use transport layer security\n"
            "  -user      switch to user after opening listening socket\n"
            "  -daemon    start as a daemon\n"
            "  -pid-file  write PID in given file\n"
        ) << toolName;
    }

    return exitCode;
}
