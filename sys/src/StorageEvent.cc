/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <libudev.h>
#include <cc/Format>
#include <cc/sys/StorageEvent>

namespace cc {
namespace sys {

String str(StorageAction action)
{
    switch (action) {
        case StorageAction::Add:    return "StorageAction::Add";
        case StorageAction::Remove: return "StorageAction::Remove";
        case StorageAction::Change: return "StorageAction::Change";
        case StorageAction::Move:   return "StorageAction::Move";
        case StorageAction::Present:return "StorageAction::Present";
    }
    return String{};
}

StorageEvent::StorageEvent(struct udev_device *dev)
{
    {
        const char *s = udev_device_get_action(dev);
        if (s) {
            if (strcmp(s, "add") == 0) action_ = StorageAction::Add;
            else if (strcmp(s, "remove") == 0) action_ = StorageAction::Remove;
            else if (strcmp(s, "change") == 0) action_ = StorageAction::Change;
            else if (strcmp(s, "move") == 0) action_ = StorageAction::Move;
        }
    }

    sysPath_ = udev_device_get_syspath(dev);
    devNode_ = udev_device_get_devnode(dev);

    struct udev_list_entry *root           = udev_device_get_properties_list_entry(dev);
    struct udev_list_entry *vendorEntry    = udev_list_entry_get_by_name(root, "ID_VENDOR");
    struct udev_list_entry *modelEntry     = udev_list_entry_get_by_name(root, "ID_MODEL");
    struct udev_list_entry *vendorIdEntry  = udev_list_entry_get_by_name(root, "ID_VENDOR_ID");
    struct udev_list_entry *modelIdEntry   = udev_list_entry_get_by_name(root, "ID_MODEL_ID");
    struct udev_list_entry *serialEntry    = udev_list_entry_get_by_name(root, "ID_SERIAL_SHORT");
    if (!serialEntry) serialEntry          = udev_list_entry_get_by_name(root, "ID_SERIAL");
    struct udev_list_entry *fsTypeEntry    = udev_list_entry_get_by_name(root, "ID_FS_TYPE");
    struct udev_list_entry *fsVersionEntry = udev_list_entry_get_by_name(root, "ID_FS_VERSION");
    struct udev_list_entry *fsLabelEntry   = udev_list_entry_get_by_name(root, "ID_FS_LABEL");

    if (vendorEntry)    vendor_    = udev_list_entry_get_value(vendorEntry);
    if (modelEntry)     model_     = udev_list_entry_get_value(modelEntry);
    if (vendorIdEntry)  vendorId_  = udev_list_entry_get_value(vendorIdEntry);
    if (modelIdEntry)   modelId_   = udev_list_entry_get_value(modelIdEntry);
    if (serialEntry)    serial_    = udev_list_entry_get_value(serialEntry);
    if (fsTypeEntry)    fsType_    = udev_list_entry_get_value(fsTypeEntry);
    if (fsVersionEntry) fsVersion_ = udev_list_entry_get_value(fsVersionEntry);
    if (fsLabelEntry)   fsLabel_   = udev_list_entry_get_value(fsLabelEntry);

    {
        struct udev_list_entry *entry = udev_list_entry_get_by_name(root, "ID_USB_DRIVER");
        isUsbStorage_ = entry && (strcmp(udev_list_entry_get_value(entry), "usb-storage") == 0);
    }
}

String StorageEvent::toString() const
{
    return Format()
        << "StorageEvent {" << nl
        << "  action      : " << action_ << nl
        << "  sysPath     : " << sysPath_ << nl
        << "  devNode     : " << devNode_ << nl
        << "  serial      : " << serial_ << nl
        << "  fsType      : " << fsType_ << nl
        << "  fsVersion   : " << fsVersion_ << nl
        << "  isUsbStorage: " << isUsbStorage_ << nl
        << "  vendor      : " << vendor_ << nl
        << "  model       : " << model_ << nl
        << "  vendorId    : " << vendorId_ << nl
        << "  modelId     : " << modelId_ << nl
        << "}";
}

}} // namespace cc::sys
