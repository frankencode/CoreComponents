#include <cc/stdio>
#include <cc/debug>
#include <cc/Arguments>
#include <cc/SignalMaster>
#include <cc/sys/StorageMonitor>

using namespace cc;
using namespace cc::sys;

void runCommand(const String &command)
{
    if (command == "") return;
    CC_INSPECT(command);
}

void runMonitor(const VariantMap *options)
{
    auto serials = String(options->value("serial"))->split(",");
    String auditPath = options->value("audit");
    String attachCommand = options->value("attach");
    String detachCommand = options->value("detach");

    Thread::blockSignals(SignalSet::createFull());

    auto monitor = StorageMonitor::start();

    auto signalMaster = SignalMaster::start(
        [=](int signal, bool *fin) {
            if (signal == SIGINT || signal == SIGTERM || signal == SIGHUP) {
                monitor->shutdown();
                *fin = true;
            }
        }
    );

    for (Ref<const StorageEvent> event; monitor->events()->popFront(&event);)
    {
        fout() << event << nl;
        if (serials->count() == 0 || serials->contains(event->serial())) {
            switch (event->action()) {
                case StorageAction::Add:
                case StorageAction::Present: {
                    runCommand(attachCommand);
                    break;
                }
                case StorageAction::Remove: {
                    runCommand(detachCommand);
                    break;
                }
                default:;
            };
        }
    }

    runCommand(detachCommand);

    monitor->wait();
    signalMaster->wait();
}

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->fileName();

    try {
        auto options = VariantMap::create();
        options->insert("serial", "");
        options->insert("audit", ".audit.log");
        options->insert("attach", "");
        options->insert("detach", "");

        auto arguments = Arguments::parse(argc, argv, options);

        runMonitor(arguments->options());
    }
    catch (HelpError &) {
        fout(
            "Usage: %% [OPTION]...\n"
            "Monitor storage media events for dedicated removable storage devices (aka keys)\n"
            "\n"
            "Options:\n"
            "  -serial  comma separated list of device serial numbers\n"
            "  -audit   log file path (relative path: log is placed on the key)\n"
            "  -attach  command to execute when a key is inserted\n"
            "  -detach  command to execute when a key is removed\n"
        ) << toolName;
    }

    return 0;
}
