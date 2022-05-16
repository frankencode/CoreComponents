/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/NetworkInterface>
#include <cc/SystemError>
#ifdef __linux
#include <cc/File>
#include <cc/LineSource>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#include <cstring>
#endif

namespace cc {

NetworkInterface NetworkInterface::query(const String &name, ProtocolFamily family)
{
    List<NetworkInterface> list = queryAll(family);
    for (const NetworkInterface &interface: list) {
        if (interface.name() == name)
            return interface;
    }

    return NetworkInterface{};
}

#ifdef __linux
List<NetworkInterface> NetworkInterface::queryAll(ProtocolFamily family)
{
    List<NetworkInterface> list;
    getLink(&list);

    int families[2];
    families[0] = (+family == AF_UNSPEC || +family == AF_INET6) ? AF_INET6 : -1;
    families[1] = (+family == AF_UNSPEC || +family == AF_INET ) ? AF_INET  : -1;

    for (int j = 0; j < 2; ++j)
    {
        if (families[j] == -1) continue;
        family = static_cast<ProtocolFamily>(families[j]);

        int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
        if (fd == -1) CC_SYSTEM_DEBUG_ERROR(errno);
        int seq = 0;

        struct sockaddr_nl src;
        std::memset(&src, 0, sizeof(src));
        src.nl_family = AF_NETLINK;
        src.nl_pid = ::getpid();
        if (::bind(fd, (struct sockaddr *)&src, (socklen_t)sizeof(src)) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);

        // send request
        {
            size_t msgLen = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
            Bytes buf = Bytes::allocate(static_cast<long>(msgLen));
            struct nlmsghdr *msg = (struct nlmsghdr *)buf.items();
            if (!msg) CC_SYSTEM_DEBUG_ERROR(errno);

            std::memset(msg, 0, msgLen);
            msg->nlmsg_type = RTM_GETADDR;
            msg->nlmsg_len = msgLen;
            msg->nlmsg_flags = NLM_F_REQUEST|NLM_F_ROOT;
            msg->nlmsg_pid = src.nl_pid;
            msg->nlmsg_seq = seq++;

            struct ifaddrmsg *data = (struct ifaddrmsg *)NLMSG_DATA(msg);
            data->ifa_family = +family;

            struct sockaddr_nl dst;
            std::memset(&dst, 0, sizeof(dst));
            dst.nl_family = AF_NETLINK;

            struct msghdr hdr;
            std::memset(&hdr, 0, sizeof(hdr));

            struct iovec iov = { msg, msgLen };
            hdr.msg_iov = &iov;
            hdr.msg_iovlen = 1;
            hdr.msg_name = (void *)&dst;
            hdr.msg_namelen = sizeof(dst);

            if (::sendmsg(fd, &hdr, 0/*flags*/) == -1)
                CC_SYSTEM_DEBUG_ERROR(errno);
        }

        // process reply
        {
            struct msghdr hdr;
            std::memset(&hdr, 0, sizeof(hdr));

            ssize_t bufSize = ::recvmsg(fd, &hdr, MSG_PEEK|MSG_TRUNC);
            if (bufSize < 0) CC_SYSTEM_DEBUG_ERROR(errno);

            Bytes buf = Bytes::allocate(bufSize);

            struct iovec iov = { buf.items(), (size_t)buf.count() };
            hdr.msg_iov = &iov;
            hdr.msg_iovlen = 1;
            ssize_t bufFill = ::recvmsg(fd, &hdr, 0/*flags*/);

            if (::close(fd) == -1)
                CC_SYSTEM_DEBUG_ERROR(errno);

            struct nlmsghdr *msg = (struct nlmsghdr *)buf.items();

            for (;NLMSG_OK(msg, unsigned(bufFill)); msg = NLMSG_NEXT(msg, bufFill))
            {
                unsigned msgType = msg->nlmsg_type;
                // unsigned msgFlags = msg->nlmsg_flags;

                if (msgType == RTM_NEWADDR)
                {
                    struct ifaddrmsg *data = (struct ifaddrmsg *)NLMSG_DATA(msg);
                    struct rtattr *attr = (struct rtattr *)IFA_RTA(data);
                    int attrFill = NLMSG_PAYLOAD(msg, sizeof(struct ifaddrmsg));

                    NetworkInterface interface;
                    NetworkLabel label;

                    for (const NetworkInterface &candidate: list) {
                        if (static_cast<unsigned>(candidate->index_) == data->ifa_index) {
                            interface = candidate;
                        }
                    }

                    if (!interface) {
                        if (!getLink(&list, data->ifa_index)) continue;
                        if (list.count() == 0) continue;
                        interface = list.last();
                    }

                    for (;RTA_OK(attr, attrFill); attr = RTA_NEXT(attr, attrFill))
                    {
                        unsigned attrType = attr->rta_type;
                        // unsigned attrLen = RTA_PAYLOAD(attr);

                        if (
                            attrType == IFA_ADDRESS   ||
                            attrType == IFA_LOCAL     ||
                            attrType == IFA_BROADCAST ||
                            attrType == IFA_ANYCAST
                        ) {
                            SocketAddress address;
                            struct sockaddr_in addr4;
                            struct sockaddr_in6 addr6;
                            if (data->ifa_family == AF_INET) {
                                std::memset(&addr4, 0, sizeof(addr4));
                                // addr.sin_len = sizeof(addr);
                                *(uint8_t *)&addr4 = sizeof(addr4); // uggly, but safe HACK, for BSD4.4
                                addr4.sin_family = AF_INET;
                                addr4.sin_addr = *(struct in_addr *)RTA_DATA(attr);
                                if (attrType != IFA_ADDRESS)
                                    address = SocketAddress{&addr4};
                            }
                            else if (data->ifa_family == AF_INET6) {
                                memset(&addr6, 0, sizeof(addr6));
                                #ifdef SIN6_LEN
                                addr.sin6_len = sizeof(addr6);
                                #endif
                                addr6.sin6_family = AF_INET6;
                                addr6.sin6_addr = *(struct in6_addr *)RTA_DATA(attr);
                                addr6.sin6_scope_id = data->ifa_scope;
                                if (attrType != IFA_ADDRESS)
                                    address = SocketAddress{&addr6};
                            }
                            if (attrType == IFA_ADDRESS) {
                                if (data->ifa_family == AF_INET)
                                    label = NetworkLabel{SocketAddress{&addr4}};
                                else if (data->ifa_family == AF_INET6)
                                    label = NetworkLabel{SocketAddress{&addr6}};
                                interface->addressList_.append(label);
                            }
                            if ((!label.isNull()) && (data->ifa_family == AF_INET)) {
                                if (attrType == IFA_LOCAL)
                                    label->localAddress_ = address;
                                else if (attrType == IFA_BROADCAST)
                                    label->broadcastAddress_ = address;
                                else if (attrType == IFA_ANYCAST)
                                    label->anycastAddress_ = address;
                            }
                        }
                    }
                }
                else if (msgType == NLMSG_DONE) { // paranoid HACK
                    break;
                }
            }
        }
    }

    return list;
}

NetworkInterface NetworkInterface::getLink(Out<List<NetworkInterface>> list, int index)
{
    NetworkInterface firstFound;

    {
        int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
        if (fd == -1) CC_SYSTEM_DEBUG_ERROR(errno);
        int seq = 0;

        struct sockaddr_nl src;
        std::memset(&src, 0, sizeof(src));
        src.nl_family = AF_NETLINK;
        src.nl_pid = ::getpid();
        if (::bind(fd, (struct sockaddr *)&src, (socklen_t)sizeof(src)) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);

        // send request
        {
            size_t msgLen = NLMSG_LENGTH(sizeof(struct ifinfomsg));
            Bytes buffer = Bytes::allocate(static_cast<long>(msgLen));
            struct nlmsghdr *msg = (struct nlmsghdr *)buffer.items();

            std::memset(msg, 0, msgLen);
            msg->nlmsg_type = RTM_GETLINK;
            msg->nlmsg_len = msgLen;
            msg->nlmsg_flags = NLM_F_REQUEST|(NLM_F_ROOT * (index == -1));
            msg->nlmsg_pid = src.nl_pid;
            msg->nlmsg_seq = seq++;

            struct ifinfomsg *data = (struct ifinfomsg *)NLMSG_DATA(msg);
            data->ifi_family = AF_UNSPEC;
            data->ifi_change = 0xFFFFFFFF;
            data->ifi_index = index;

            struct sockaddr_nl dst;
            std::memset(&dst, 0, sizeof(dst));
            dst.nl_family = AF_NETLINK;

            struct msghdr hdr;
            std::memset(&hdr, 0, sizeof(hdr));

            struct iovec iov = { msg, msgLen };
            hdr.msg_iov = &iov;
            hdr.msg_iovlen = 1;
            hdr.msg_name = (void *)&dst;
            hdr.msg_namelen = sizeof(dst);

            if (::sendmsg(fd, &hdr, 0/*flags*/) == -1)
                CC_SYSTEM_DEBUG_ERROR(errno);
        }

        // process reply
        {
            struct msghdr hdr;
            std::memset(&hdr, 0, sizeof(hdr));

            ssize_t bufSize = ::recvmsg(fd, &hdr, MSG_PEEK|MSG_TRUNC);
            if (bufSize < 0) CC_SYSTEM_DEBUG_ERROR(errno);

            Bytes buf = Bytes::allocate(bufSize);

            struct iovec iov = { buf.items(), (size_t)buf.count() };
            hdr.msg_iov = &iov;
            hdr.msg_iovlen = 1;
            ssize_t bufFill = ::recvmsg(fd, &hdr, 0/*flags*/);

            struct nlmsghdr *msg = (struct nlmsghdr *)buf.items();

            for (;NLMSG_OK(msg, unsigned(bufFill)); msg = NLMSG_NEXT(msg, bufFill))
            {
                unsigned msgType = msg->nlmsg_type;
                // unsigned msgFlags = msg->nlmsg_flags;

                if (msgType == RTM_NEWLINK)
                {
                    struct ifinfomsg *data = (struct ifinfomsg *)NLMSG_DATA(msg);
                    struct rtattr *attr = (struct rtattr *)IFLA_RTA(data);
                    int attrFill = NLMSG_PAYLOAD(msg, sizeof(struct ifinfomsg));

                    NetworkInterface interface {
                        data->ifi_index,
                        data->ifi_type,
                        static_cast<NetworkInterfaceFlags>(data->ifi_flags)
                    };

                    for (;RTA_OK(attr, attrFill); attr = RTA_NEXT(attr, attrFill))
                    {
                        unsigned attrType = attr->rta_type;
                        unsigned attrLen = RTA_PAYLOAD(attr);

                        if ((attrType == IFLA_ADDRESS) || (attrType == IFLA_BROADCAST)) {
                            if (attrType == IFLA_ADDRESS)
                                interface->hardwareAddress_ = String{(const char *)RTA_DATA(attr), attrLen};
                            //else if (attrType == IFLA_BROADCAST)
                            //  interface->broadcastAddress_ = h;
                        }
                        else if (attrType == IFLA_IFNAME) {
                            interface->name_ = (char *)RTA_DATA(attr);
                        }
                        else if (attrType == IFLA_MTU) {
                            if (attrLen == 4)
                                interface->mtu_ = *(uint32_t *)RTA_DATA(attr);
                        }
                    }

                    if (!firstFound) firstFound = interface;
                    if (list != None{}) list->append(interface);
                    else break;
                }
            }
        }

        if (::close(fd) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }

    return firstFound;
}

#else

List<NetworkInterface> NetworkInterface::queryAll(ProtocolFamily)
{
    #warning "NetworkInterface::queryAll() not supported on your platform"
    return List<NetworkInterface>{};
}

#endif

} // namespace cc
