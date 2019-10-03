/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/can/CanSocket>
#include <cc/SystemStream>
#include <cc/exceptions>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <strings.h>

namespace cc {
namespace can {

Ref<CanSocket> CanSocket::open(const String &interface)
{
    return new CanSocket(interface);
}

CanSocket::CanSocket(const String &interface):
    interface_{interface}
{
    int fd = ::socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (fd == -1) CC_SYSTEM_DEBUG_ERROR(errno);
    socket_ = SystemStream::create(fd);

    struct sockaddr_can addr;
    memclr(&addr, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = lookupInterfaceIndex(fd, interface);

    if (::bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
        CC_SYSTEM_ERROR(errno, interface);
}

CanSocket::~CanSocket()
{}

int CanSocket::lookupInterfaceIndex(int fd, const String &interface)
{
    struct ifreq ifr;
    strncpy(ifr.ifr_name, interface, sizeof(ifr.ifr_name));
    ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = 0;
    if (::ioctl(fd, SIOCGIFINDEX, &ifr) == -1)
        CC_SYSTEM_ERROR(errno, interface);
    return ifr.ifr_ifindex;
}

void CanSocket::enableLoopback(bool on)
{
    int value = on;
    if (::setsockopt(socket_->fd(), SOL_CAN_RAW, CAN_RAW_LOOPBACK, &value, sizeof(value)) == -1)
        CC_SYSTEM_ERROR(errno, interface_);
}

void CanSocket::enableErrorFrames(bool on)
{
    can_err_mask_t errorMask = CAN_ERR_MASK;
    if (::setsockopt(socket_->fd(), SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &errorMask, sizeof(errorMask)) == -1)
        CC_SYSTEM_ERROR(errno, interface_);
}

bool CanSocket::waitFrame(int timeout)
{
    return socket_->waitFor(IoReady::Read, timeout);
}

bool CanSocket::readFrame(CanFrame *frame)
{
    int n = 0;
    while (true) {
        n = ::read(socket_->fd(), static_cast<StructCanFrame *>(frame), sizeof(StructCanFrame));
        if (n == -1) {
            if (errno != EINTR)
                CC_SYSTEM_ERROR(errno, interface_);
        }
        else
            break;
    }

    return n > 0;
}

void CanSocket::writeFrame(const CanFrame *frame)
{
    while (true) {
        if (::write(socket_->fd(), static_cast<const StructCanFrame *>(frame), sizeof(StructCanFrame)) == -1) {
            if (errno != EINTR)
                CC_SYSTEM_ERROR(errno, interface_);
        }
        else
            break;
    }
}

}} // namespace cc::can
