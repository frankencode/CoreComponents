/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/exceptions>
#include <cc/Arguments>
#include <cc/IoMonitor>
#include <cc/net/Uri>
#include <cc/net/StreamSocket>

using namespace cc;
using namespace cc::net;

void dump(const String &prefix, const CharArray *data)
{
    StringList parts = data->split('\n');
    for (int i = 0; i < parts->count(); ++i) {
        if (i != parts->count() - 1)
            parts->at(i) = parts->at(i) + "\n";
        if (parts->at(i)->count() == 0) continue;
        parts->at(i) = prefix + " " + parts->at(i)->escape() + "\n";
    }
    fout() << parts->join();
    ferr() << "-----\n";
}

int main(int argc, char **argv)
{
    String toolName = String{argv[0]}->fileName();

    try {
        StringList items = Arguments{argc, argv}->read();
        if (items->count() != 2) throw HelpRequest{};

        auto proxyAddress = SocketAddress::resolveUri(items->at(0));
        auto origAddress  = SocketAddress::resolveUri(items->at(1));

        StreamSocket listenSocket;
        listenSocket->listen(proxyAddress);

        ferr()
            << "Listening at " << proxyAddress << " to intercept messages to " << origAddress << nl
            << "..." << nl;

        while (true)
        {
            StreamSocket serverSocket = listenSocket->accept();

            StreamSocket clientSocket;
            clientSocket->connect(origAddress);

            ferr()
                << "~~~~~ CONNECTION ESTABLISHED" << nl
                << ":srv: " << serverSocket->address() << nl
                << ":cli: " << clientSocket->address() << nl
                << "~~~~~" << nl;

            Ref<IoMonitor> monitor = IoMonitor::create(2);
            monitor->addEvent(IoReady::Read, serverSocket);
            monitor->addEvent(IoReady::Read, clientSocket);

            String data = String::allocate(0x10000);
            bool serverClosed = false;
            bool clientClosed = false;
            while (!(serverClosed || clientClosed)) {
                IoActivity activity = monitor->wait();
                for (const IoEvent *event: activity) {
                    StreamSocket socket = event->target();
                    if (socket == serverSocket) {
                        int n = serverSocket->read(mutate(data));
                        if (n == 0) {
                            serverClosed = true;
                            ferr() << "[srv] DONE" << nl;
                            continue;
                        }
                        String range = data->select(0, n);
                        clientSocket->write(range);
                        dump("(cli)", range);
                    }
                    else if (socket == clientSocket) {
                        int n = clientSocket->read(mutate(data));
                        if (n == 0) {
                            clientClosed = true;
                            ferr() << "[cli] DONE" << nl;
                            continue;
                        }
                        String range = data->select(0, n);
                        serverSocket->write(range);
                        dump("(srv)", range);
                    }
                    else break;
                }
            }

            ferr() << "~~~~~ CONNECTION CLOSED" << nl << nl;
        }
    }
    catch (UsageError &ex) {
        fout()
            << ex.message() << nl
            << nl
            << "See --help for usage information" << nl;
        return 1;
    }
    catch (HelpRequest &) {
        fout(
            "Usage: %% [OPTION]... <proxy-server:port> <orig-server:port>\n"
            "Tap into and dump TCP client/server communication\n"
            "\n"
        ) << toolName;
        return 1;
    }

    return 0;
}
