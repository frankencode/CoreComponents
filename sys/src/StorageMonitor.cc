/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <libudev.h>
#include <cc/debug>
#include <cc/IoMonitor>
#include <cc/SystemStream>
#include <cc/sys/StorageMonitor>

namespace cc {
namespace sys {

Ref<StorageMonitor> StorageMonitor::start()
{
    Ref<StorageMonitor> monitor = new StorageMonitor{};
    monitor->Thread::start();
    return monitor;
}

StorageMonitor::StorageMonitor():
    events_{Events::create()}
{
    SystemStream::connect(&controlMaster_, &controlSlave_);
}

void StorageMonitor::shutdown()
{
    controlMaster_->write("shutdown");
}

void StorageMonitor::printDebug(struct udev_device *device)
{
    CC_INSPECT(udev_device_get_action(device));
    CC_INSPECT(udev_device_get_devpath(device));
    CC_INSPECT(udev_device_get_subsystem(device));

    struct udev_list_entry *list_entry;

    udev_list_entry_foreach(list_entry, udev_device_get_properties_list_entry(device))

    CC_DEBUG << udev_list_entry_get_name(list_entry) << "=" << udev_list_entry_get_value(list_entry);
}

void StorageMonitor::enumerate(struct udev *udev)
{
    struct udev_enumerate* enumerate = udev_enumerate_new(udev);

    udev_enumerate_add_match_subsystem(enumerate, "block");
    udev_enumerate_add_match_property(enumerate, "DEVTYPE", "partition");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    udev_list_entry_foreach(entry, devices)
    {
        String sysPath = udev_list_entry_get_name(entry);
        struct udev_device *dev = udev_device_new_from_syspath(udev, sysPath);

        events_->pushBack(Object::create<StorageEvent>(dev));

        udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
}

void StorageMonitor::run()
{
    try {
        struct udev *udev = udev_new();
        struct udev_monitor *mon = udev_monitor_new_from_netlink(udev, "udev");
        udev_monitor_filter_add_match_subsystem_devtype(mon, "block", "partition");
        udev_monitor_enable_receiving(mon);
        Ref<SystemStream> monitorStream = SystemStream::duplicate(udev_monitor_get_fd(mon));

        Ref<IoMonitor> ioMonitor = IoMonitor::create(2);
        const IoEvent *shutdownEvent = ioMonitor->addEvent(IoReady::Read, controlSlave_);
        const IoEvent *monitorEvent = ioMonitor->addEvent(IoReady::Read, monitorStream);

        enumerate(udev);

        while (true)
        {
            Ref<IoActivity> activity = ioMonitor->wait();
            if (activity->contains(shutdownEvent)) break;

            if (activity->contains(monitorEvent))
            {
                struct udev_device *dev = udev_monitor_receive_device(mon);
                if (!dev) continue;

                // printDebug(dev);

                events_->pushBack(Object::create<StorageEvent>(dev));

                udev_device_unref(dev);
            }
        }

        events_->pushBack(nullptr);

        udev_monitor_unref(mon);
        udev_unref(udev);
    }
    catch (Exception &ex) {
        CC_DEBUG << ex;
    }
}

}} // namespace cc::sys
