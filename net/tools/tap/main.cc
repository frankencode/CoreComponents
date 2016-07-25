/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/exceptions>
#include <cc/Arguments>
#include <cc/IoMonitor>
#include <cc/net/StreamSocket>

using namespace cc;
using namespace cc::net;

void dump(String prefix, ByteArray *data)
{
    Ref<StringList> parts = data->split('\n');
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
    String toolName = String(argv[0])->fileName();

    try {
        Ref<Arguments> arguments = Arguments::parse(argc, argv);
        arguments->validate(VariantMap::create());
        Ref<StringList> items = arguments->items();
        if (items->count() != 2) throw HelpError();

        Ref<SocketAddress> proxyAddress = SocketAddress::read(items->at(0));
        Ref<SocketAddress> origAddress  = SocketAddress::read(items->at(1));

        Ref<StreamSocket> listenSocket = StreamSocket::listen(proxyAddress);

        ferr()
            << "Listening at " << proxyAddress << " to intercept messages to " << origAddress << nl
            << "..." << nl;

        while (true)
        {
            Ref<StreamSocket> serverSocket = listenSocket->accept();
            Ref<StreamSocket> clientSocket = StreamSocket::connect(origAddress);

            ferr()
                << "~~~~~ CONNECTION ESTABLISHED" << nl
                << ":srv: " << serverSocket->address() << nl
                << ":cli: " << clientSocket->address() << nl
                << "~~~~~" << nl;

            Ref<IoMonitor> monitor = IoMonitor::create(2);
            monitor->addEvent(SystemIo::ReadyRead, serverSocket);
            monitor->addEvent(SystemIo::ReadyRead, clientSocket);

            Ref<ByteArray> data = ByteArray::allocate(0x10000);
            bool serverClosed = false;
            bool clientClosed = false;
            while (!(serverClosed || clientClosed)) {
                Ref<IoActivity> activity = monitor->poll();
                for (int i = 0; i < activity->count(); ++i) {
                    const IoEvent *event = activity->at(i);
                    Ref<StreamSocket> socket = event->target();
                    if (socket == serverSocket) {
                        int n = serverSocket->read(data);
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
                        int n = clientSocket->read(data);
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
    catch (HelpError &) {
        fout(
            "Usage: %% [OPTION]... <proxy-server:port> <orig-server:port>\n"
            "Tap into and dump TCP client/server communication\n"
            "\n"
        ) << toolName;
        return 1;
    }

    return 0;
}
