/*
 * Copyright (C) 2019-2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/CanSocket>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can/raw.h>
#include <unistd.h>
#include <cstring>

namespace cc {

struct CanSocket::State final: public CanMedia::State
{
    explicit State(const String &interface):
        socket_{::socket(PF_CAN, SOCK_RAW, CAN_RAW)}
    {
        if (socket_.fd() == -1) CC_SYSTEM_DEBUG_ERROR(errno);

        struct sockaddr_can addr;
        std::memset(&addr, 0, sizeof(addr));
        addr.can_family = AF_CAN;
        addr.can_ifindex = lookupInterface(interface);

        if (::bind(socket_.fd(), (struct sockaddr *)&addr, sizeof(addr)) == -1) {
            CC_SYSTEM_ERROR(errno, interface);
        }
    }

    int lookupInterface(const String &interface)
    {
        struct ifreq ifr;
        std::strncpy(ifr.ifr_name, interface, sizeof(ifr.ifr_name));
        ifr.ifr_name[sizeof(ifr.ifr_name) - 1] = 0;
        if (::ioctl(socket_.fd(), SIOCGIFINDEX, &ifr) == -1) {
            CC_SYSTEM_ERROR(errno, interface);
        }
        return ifr.ifr_ifindex;
    }

    void loopback(bool on)
    {
        int value = on;
        if (::setsockopt(socket_.fd(), SOL_CAN_RAW, CAN_RAW_LOOPBACK, &value, sizeof(value)) == -1) {
            CC_SYSTEM_DEBUG_ERROR(errno);
        }
    }

    void errors(bool on)
    {
        can_err_mask_t errorMask = CAN_ERR_MASK;
        if (::setsockopt(socket_.fd(), SOL_CAN_RAW, CAN_RAW_ERR_FILTER, &errorMask, sizeof(errorMask)) == -1) {
            CC_SYSTEM_DEBUG_ERROR(errno);
        }
    }

    bool waitFrame(int timeout = -1) override
    {
        return socket_.wait(IoEvent::ReadyRead, timeout);
    }

    bool read(Out<CanFrame> frame) override
    {
        ssize_t n = 0;

        while (true) {
            n = ::read(socket_.fd(), frame->frame(), frame->frameSize());
            if (n == -1) {
                if (errno != EINTR)
                    CC_SYSTEM_DEBUG_ERROR(errno);
            }
            else
                break;
        }

        return n > 0;
    }

    void write(const CanFrame &frame) override
    {
        while (true) {
            if (::write(socket_.fd(), frame.frame(), frame.frameSize()) == -1) {
                if (errno != EINTR)
                    CC_SYSTEM_DEBUG_ERROR(errno);
            }
            else
                break;
        }
    }

    IoStream socket_;
};

CanSocket::CanSocket(const String &interface):
    CanMedia{new State{interface}}
{}

CanSocket &CanSocket::loopback(bool on)
{
    me().loopback(on);
    return *this;
}

CanSocket &CanSocket::errors(bool on)
{
    me().errors(on);
    return *this;
}

CanSocket::State &CanSocket::me()
{
    return Object::me.as<State>();
}

const CanSocket::State &CanSocket::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
