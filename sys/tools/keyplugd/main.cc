#include <cc/stdio>
#include <cc/debug>
#include <cc/Arguments>
#include <cc/SignalMaster>
#include <cc/sys/StorageMonitor>

using namespace cc;
using namespace cc::sys;

enum class KeyAction { Attach, Detach };

void runCommand(KeyAction action, const String &command)
{
    if (command == "") return;
    CC_INSPECT(command);
}

void runMonitor(const VariantMap *options)
{
    auto serials = String(options->value("serial"))->split(",");
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
                    runCommand(KeyAction::Attach, attachCommand);
                    break;
                }
                case StorageAction::Remove: {
                    runCommand(KeyAction::Detach, detachCommand);
                    break;
                }
                default:;
            };
        }
    }

    runCommand(KeyAction::Detach, detachCommand);

    monitor->wait();
    signalMaster->wait();
}

int main(int argc, char **argv)
{
    String toolName = String(argv[0])->fileName();

    try {
        auto options = VariantMap::create();
        options->insert("serial", "");
        options->insert("attach", "");
        options->insert("detach", "");

        auto arguments = Arguments::parse(argc, argv, options);

        runMonitor(arguments->options());
    }
    catch (HelpError &) {
        fout(
            "Usage: %% [OPTION]...\n"
            "Auto mount manager for smart keys\n"
            "\n"
            "Options:\n"
            "  -serial  comma separated list of device serial numbers\n"
            "  -attach  command to execute when a smart key is inserted (CWD = mount point)\n"
            "  -detach  command to execute when a smart key is removed\n"
        ) << toolName;
    }

    return 0;
}
