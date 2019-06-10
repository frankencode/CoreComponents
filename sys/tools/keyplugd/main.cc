#include <cc/stdio>
#include <cc/debug>
#include <cc/Arguments>
#include <cc/SignalMaster>
#include <cc/Process>
#include <cc/Dir>
#include <cc/System>
#include <cc/sys/StorageMonitor>

using namespace cc;
using namespace cc::sys;

class MountGuard {
public:
    MountGuard(const String &devNode, const String &fsType, const String &mountOptions):
        mountPath_{Dir::createTemp()},
        cwdSaved_{Process::getCwd()}
    {
        Process::cd(mountPath_);
        System::mount(devNode, mountPath_, fsType, mountOptions);
    }

    ~MountGuard()
    {
        System::unmount(mountPath_);
        Dir::remove(mountPath_);
        Process::cd(cwdSaved_);
    }

    String mountPath() const { return mountPath_; }

    const MountGuard *operator->() const { return this; }

private:
    String mountPath_;
    String cwdSaved_;
};

void runAttachCommand(const String &shellCommand, const String &devNode, const String &fsType, const String &mountOptions)
{
    if (shellCommand == "") return;

    MountGuard guard{devNode, fsType, mountOptions};

    Process::stage()
        ->setArgs(
            StringList::create()
                << Process::getEnv("SHELL")
                << "-c"
                << shellCommand
        )
        ->start()
        ->wait();
}

void runDetachCommand(const String &shellCommand)
{
    if (shellCommand == "") return;

    Process::stage()
        ->setArgs(
            StringList::create()
                << Process::getEnv("SHELL")
                << "-c"
                << shellCommand
        )
        ->start()
        ->wait();
}

void runMonitor(const VariantMap *options)
{
    auto serials = String(options->value("serial"))->split(",");
    String attachCommand = options->value("attach");
    String detachCommand = options->value("detach");
    String mountOptions = options->value("options");
    bool verbose = options->value("verbose");

    Thread::blockSignals(SignalSet::createFull());

    auto monitor = StorageMonitor::start();

    auto signalMaster = SignalMaster::start(
        [=](Signal signal, bool *fin) {
            if (+signal == SIGINT || +signal == SIGTERM || +signal == SIGHUP) {
                monitor->shutdown();
                *fin = true;
            }
        }
    );

    for (Ref<const StorageEvent> event; monitor->events()->popFront(&event);)
    {
        if (verbose) fout() << event << nl;

        if (serials->count() == 0 || serials->contains(event->serial())) {
            switch (event->action()) {
                case StorageAction::Add:
                case StorageAction::Present: {
                    runAttachCommand(attachCommand, event->devNode(), event->fsType(), mountOptions);
                    break;
                }
                case StorageAction::Remove: {
                    runDetachCommand(detachCommand);
                    break;
                }
                default:;
            };
        }
    }

    runDetachCommand(detachCommand);

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
        options->insert("options", "");
        options->insert("verbose", false);

        auto arguments = Arguments::parse(argc, argv, options);

        runMonitor(arguments->options());
    }
    catch (HelpError &) {
        fout(
            "Usage: %% [OPTION]...\n"
            "Auto mount manager for smart keys\n"
            "\n"
            "Options:\n"
            "  -serial   comma separated list of device serial numbers\n"
            "  -attach   command to execute when a smart key is inserted (CWD = mount point)\n"
            "  -detach   command to execute when a smart key is removed\n"
            "  -options  mount options\n"
            "  -verbose  print more verbose information (storage events, etc.)\n"
        ) << toolName;
    }

    return 0;
}
