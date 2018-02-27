/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#ifdef __linux
#include <sys/types.h>
#include <sys/socket.h> // socket, sendmsg, recvmsg
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <errno.h>
#else
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/socket.h>
#include <net/if_dl.h>
#include <net/route.h>
#include <errno.h>
#endif

#include <unistd.h> // getpid
#ifdef __linux
#include <cc/File>
#include <cc/LineSource>
#endif
#include <cc/exceptions>
#include <cc/net/NetworkInterface>

namespace cc {
namespace net {

Ref<NetworkInterface> NetworkInterface::create()
{
    return new NetworkInterface;
}

Ref<NetworkInterface> NetworkInterface::query(String name, int family)
{
    Ref<NetworkInterfaceList> list = queryAll(family);
    for (auto interface: list) {
        if (interface->name() == name)
            return interface;
    }

    return NetworkInterface::create();
}

#ifdef __linux
Ref<NetworkInterfaceList> NetworkInterface::queryAll(int family)
{
    Ref<NetworkInterfaceList> list = NetworkInterfaceList::create();
    getLink(list);

    int families[2];
    families[0] = ((family == AF_UNSPEC) || (family == AF_INET6)) ? AF_INET6 : -1;
    families[1] = ((family == AF_UNSPEC) || (family == AF_INET)) ? AF_INET : -1;

    for (int j = 0; j < 2; ++j)
    {
        if (families[j] == -1) continue;
        family = families[j];

        int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
        if (fd == -1) CC_SYSTEM_DEBUG_ERROR(errno);
        int seq = 0;

        struct sockaddr_nl src;
        memclr(&src, sizeof(src));
        src.nl_family = AF_NETLINK;
        src.nl_pid = ::getpid();
        if (::bind(fd, (struct sockaddr *)&src, (socklen_t)sizeof(src)) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);

        // send request
        {
            size_t msgLen = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
            struct nlmsghdr *msg = (struct nlmsghdr *)cc::malloc(msgLen);
            if (!msg) CC_SYSTEM_DEBUG_ERROR(errno);

            memclr(msg, msgLen);
            msg->nlmsg_type = RTM_GETADDR;
            msg->nlmsg_len = msgLen;
            msg->nlmsg_flags = NLM_F_REQUEST|NLM_F_ROOT;
            msg->nlmsg_pid = src.nl_pid;
            msg->nlmsg_seq = seq++;

            struct ifaddrmsg *data = (struct ifaddrmsg *)NLMSG_DATA(msg);
            data->ifa_family = family;

            struct sockaddr_nl dst;
            memclr(&dst, sizeof(dst));
            dst.nl_family = AF_NETLINK;

            struct msghdr hdr;
            memclr(&hdr, sizeof(hdr));

            struct iovec iov = { msg, msgLen };
            hdr.msg_iov = &iov;
            hdr.msg_iovlen = 1;
            hdr.msg_name = (void *)&dst;
            hdr.msg_namelen = sizeof(dst);

            if (::sendmsg(fd, &hdr, 0/*flags*/) == -1)
                CC_SYSTEM_DEBUG_ERROR(errno);

            cc::free(msg);
        }

        // process reply
        {
            struct msghdr hdr;
            memclr(&hdr, sizeof(hdr));

            ssize_t bufSize = ::recvmsg(fd, &hdr, MSG_PEEK|MSG_TRUNC);
            if (bufSize < 0) CC_SYSTEM_DEBUG_ERROR(errno);

            void *buf = cc::malloc(bufSize);
            if (!buf) CC_SYSTEM_DEBUG_ERROR(errno);

            struct iovec iov = { buf, (size_t)bufSize };
            hdr.msg_iov = &iov;
            hdr.msg_iovlen = 1;
            ssize_t bufFill = ::recvmsg(fd, &hdr, 0/*flags*/);

            if (::close(fd) == -1)
                CC_SYSTEM_DEBUG_ERROR(errno);

            struct nlmsghdr *msg = (struct nlmsghdr *)buf;

            for (;NLMSG_OK(msg, unsigned(bufFill)); msg = NLMSG_NEXT(msg, bufFill))
            {
                unsigned msgType = msg->nlmsg_type;
                // unsigned msgFlags = msg->nlmsg_flags;

                if (msgType == RTM_NEWADDR)
                {
                    struct ifaddrmsg *data = (struct ifaddrmsg *)NLMSG_DATA(msg);
                    struct rtattr *attr = (struct rtattr *)IFA_RTA(data);
                    int attrFill = NLMSG_PAYLOAD(msg, sizeof(struct ifaddrmsg));

                    Ref<SocketAddress> label;
                    Ref<NetworkInterface> interface = 0;

                    for (int i = 0; i < list->count(); ++i) {
                        if (unsigned(list->at(i)->index_) == data->ifa_index) {
                            interface = list->at(i);
                            break;
                        }
                    }

                    if (!interface) {
                        if (!getLink(list, data->ifa_index))
                            continue;
                        interface = list->at(list->count() - 1);
                    }

                    for (;RTA_OK(attr, attrFill); attr = RTA_NEXT(attr, attrFill))
                    {
                        unsigned attrType = attr->rta_type;
                        // unsigned attrLen = RTA_PAYLOAD(attr);

                        if ( (attrType == IFA_ADDRESS)   ||
                             (attrType == IFA_LOCAL)     ||
                             (attrType == IFA_BROADCAST) ||
                             (attrType == IFA_ANYCAST)
                           )
                        {
                            Ref<SocketAddress> address;
                            struct sockaddr_in addr4;
                            struct sockaddr_in6 addr6;
                            if (data->ifa_family == AF_INET) {
                                memclr(&addr4, sizeof(addr4));
                                // addr.sin_len = sizeof(addr);
                                *(uint8_t *)&addr4 = sizeof(addr4); // uggly, but safe HACK, for BSD4.4
                                addr4.sin_family = AF_INET;
                                addr4.sin_addr = *(struct in_addr *)RTA_DATA(attr);
                                if (attrType != IFA_ADDRESS)
                                    address = SocketAddress::create(&addr4);
                            }
                            else if (data->ifa_family == AF_INET6) {
                                memclr(&addr6, sizeof(addr6));
                                #ifdef SIN6_LEN
                                addr.sin6_len = sizeof(addr6);
                                #endif
                                addr6.sin6_family = AF_INET6;
                                addr6.sin6_addr = *(struct in6_addr *)RTA_DATA(attr);
                                addr6.sin6_scope_id = data->ifa_scope;
                                if (attrType != IFA_ADDRESS)
                                    address = SocketAddress::create(&addr6);
                            }
                            if (attrType == IFA_ADDRESS) {
                                if (data->ifa_family == AF_INET)
                                    label = SocketAddressEntry::create(&addr4, data->ifa_prefixlen);
                                else if (data->ifa_family == AF_INET6)
                                    label = SocketAddress::create(&addr6);
                                interface->addressList_->append(label);
                            }
                            if ((label) && (data->ifa_family == AF_INET)) {
                                SocketAddressEntry *entry = Object::cast<SocketAddressEntry *>(label);
                                if (attrType == IFA_LOCAL)
                                    entry->localAddress_ = address;
                                else if (attrType == IFA_BROADCAST)
                                    entry->broadcastAddress_ = address;
                                else if (attrType == IFA_ANYCAST)
                                    entry->anycastAddress_ = address;
                            }
                        }
                    }
                }
                else if (msgType == NLMSG_DONE) { // paranoid HACK
                    break;
                }
            }

            free(buf);
        }
    }

    if (list->count() == 0)
        list = queryAllIoctl(family);

    return list;
}

NetworkInterface::NetworkInterface():
    index_(-1),
    type_(0),
    flags_(0),
    hardwareAddress_(0),
    mtu_(0),
    addressList_(SocketAddressList::create())
{}

Ref<NetworkInterface> NetworkInterface::getLink(NetworkInterfaceList *list, int index)
{
    Ref<NetworkInterface> firstFound = 0;

    {
        int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
        if (fd == -1) CC_SYSTEM_DEBUG_ERROR(errno);
        int seq = 0;

        struct sockaddr_nl src;
        memclr(&src, sizeof(src));
        src.nl_family = AF_NETLINK;
        src.nl_pid = ::getpid();
        if (::bind(fd, (struct sockaddr *)&src, (socklen_t)sizeof(src)) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);

        // send request
        {
            size_t msgLen = NLMSG_LENGTH(sizeof(struct ifinfomsg));
            struct nlmsghdr *msg = (struct nlmsghdr *)cc::malloc(msgLen);
            if (!msg) CC_SYSTEM_DEBUG_ERROR(errno);

            memclr(msg, msgLen);
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
            memclr(&dst, sizeof(dst));
            dst.nl_family = AF_NETLINK;

            struct msghdr hdr;
            memclr(&hdr, sizeof(hdr));

            struct iovec iov = { msg, msgLen };
            hdr.msg_iov = &iov;
            hdr.msg_iovlen = 1;
            hdr.msg_name = (void *)&dst;
            hdr.msg_namelen = sizeof(dst);

            if (::sendmsg(fd, &hdr, 0/*flags*/) == -1)
                CC_SYSTEM_DEBUG_ERROR(errno);

            cc::free(msg);
        }

        // process reply
        {
            struct msghdr hdr;
            memclr(&hdr, sizeof(hdr));

            ssize_t bufSize = ::recvmsg(fd, &hdr, MSG_PEEK|MSG_TRUNC);
            if (bufSize < 0) CC_SYSTEM_DEBUG_ERROR(errno);

            void *buf = cc::malloc(bufSize);
            if (!buf) CC_SYSTEM_DEBUG_ERROR(errno);

            struct iovec iov = { buf, (size_t)bufSize };
            hdr.msg_iov = &iov;
            hdr.msg_iovlen = 1;
            ssize_t bufFill = ::recvmsg(fd, &hdr, 0/*flags*/);

            struct nlmsghdr *msg = (struct nlmsghdr *)buf;

            for (;NLMSG_OK(msg, unsigned(bufFill)); msg = NLMSG_NEXT(msg, bufFill))
            {
                unsigned msgType = msg->nlmsg_type;
                // unsigned msgFlags = msg->nlmsg_flags;

                if (msgType == RTM_NEWLINK)
                {
                    struct ifinfomsg *data = (struct ifinfomsg *)NLMSG_DATA(msg);
                    struct rtattr *attr = (struct rtattr *)IFLA_RTA(data);
                    int attrFill = NLMSG_PAYLOAD(msg, sizeof(struct ifinfomsg));

                    Ref<NetworkInterface> interface = NetworkInterface::create();
                    interface->index_ = data->ifi_index;
                    interface->type_  = data->ifi_type;
                    interface->flags_ = data->ifi_flags;

                    for (;RTA_OK(attr, attrFill); attr = RTA_NEXT(attr, attrFill))
                    {
                        unsigned attrType = attr->rta_type;
                        unsigned attrLen = RTA_PAYLOAD(attr);

                        if ((attrType == IFLA_ADDRESS) || (attrType == IFLA_BROADCAST)) {
                            if (attrType == IFLA_ADDRESS)
                                interface->hardwareAddress_ = CharArray::copy((const char *)RTA_DATA(attr), attrLen);
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
                    if (list) list->append(interface);
                    else break;
                }
            }

            cc::free(buf);
        }

        if (::close(fd) == -1)
            CC_SYSTEM_DEBUG_ERROR(errno);
    }

    return firstFound;
}

Ref<NetworkInterfaceList> NetworkInterface::queryAllIoctl(int family)
{
    Ref<NetworkInterfaceList> list = NetworkInterfaceList::create();

    int fd = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1) CC_SYSTEM_DEBUG_ERROR(errno);

    Ref<LineSource> source = LineSource::open(File::open("/proc/net/dev"));
    for (String line; source->read(&line);) {
        if (line->contains(':')) {
            Ref<NetworkInterface> interface = NetworkInterface::create();
            list->append(interface);
            Ref<StringList> parts = line->split(":");
            String name = parts->at(0)->trim();
            interface->name_ = name;

            {
                struct ifreq ifr;
                memclr(&ifr, sizeof(ifr));
                for (int i = 0, n = name->count(); i < n; ++i)
                    ifr.ifr_name[i] = name->at(i);

                if (::ioctl(fd, SIOCGIFHWADDR, &ifr) == -1)
                    CC_SYSTEM_DEBUG_ERROR(errno);

                interface->hardwareAddress_ = CharArray::copy((const char *)ifr.ifr_hwaddr.sa_data, 6); // quick HACK, 6 is just a safe bet

                if (::ioctl(fd, SIOCGIFFLAGS, &ifr) == -1)
                    CC_SYSTEM_DEBUG_ERROR(errno);
                interface->flags_ = ifr.ifr_flags;

                if (::ioctl(fd, SIOCGIFMTU, &ifr) == -1)
                    CC_SYSTEM_DEBUG_ERROR(errno);
                interface->mtu_ = ifr.ifr_mtu;
            }
        }
    }

    {
        struct ifconf ifc;
        int capa = 32 * sizeof(struct ifreq);
        ifc.ifc_len = capa;
        ifc.ifc_req = (struct ifreq *)cc::malloc(capa);

        while (true) {
            if (::ioctl(fd, SIOCGIFCONF, &ifc) == -1)
                CC_SYSTEM_DEBUG_ERROR(errno);
            if (ifc.ifc_len == capa) {
                cc::free(ifc.ifc_req);
                capa *= 2;
                ifc.ifc_len = capa;
                ifc.ifc_req = (struct ifreq *)cc::malloc(capa);
                continue;
            }
            break;
        }

        for (int i = 0; i < int(ifc.ifc_len / sizeof(struct ifreq)); ++i)
        {
            struct ifreq *ifr = ifc.ifc_req + i;

            if ((family != AF_UNSPEC) && (family != ifr->ifr_addr.sa_family))
                continue;

            for (int k = 0; k < list->count(); ++k)
            {
                NetworkInterface *interface = list->at(k);

                if (interface->name_ == ifr->ifr_name) {
                    struct sockaddr *addr = &ifr->ifr_addr;
                    struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
                    struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;

                    Ref<SocketAddress> label;

                    if (addr->sa_family == AF_INET)
                        label = SocketAddressEntry::create(addr4);
                    else if (addr->sa_family == AF_INET6)
                        label = SocketAddress::create(addr6);
                    interface->addressList_->append(label);

                    if ((label) && (addr->sa_family == AF_INET)) {
                        SocketAddressEntry *entry = Object::cast<SocketAddressEntry *>(label);
                        if (interface->flags_ & IFF_BROADCAST) {
                            struct ifreq ifr2 = *ifr;
                            if (::ioctl(fd, SIOCGIFBRDADDR, &ifr2) == -1)
                                CC_SYSTEM_DEBUG_ERROR(errno);
                            entry->broadcastAddress_ = SocketAddress::create((struct sockaddr_in *)&ifr2.ifr_broadaddr);
                        }
                        if (interface->flags_ & IFF_POINTOPOINT) {
                            struct ifreq ifr2 = *ifr;
                            if (::ioctl(fd, SIOCGIFDSTADDR, &ifr2) == -1)
                                CC_SYSTEM_DEBUG_ERROR(errno);
                            entry->broadcastAddress_ = SocketAddress::create((struct sockaddr_in *)&ifr2.ifr_dstaddr);
                        }
                    }
                }
            }
        }

        cc::free(ifc.ifc_req);
    }

    return list;
}

#else

#ifdef __APPLE__
#define SALIGN (sizeof(int32_t) - 1)
#else
#define SALIGN (sizeof(long) - 1)
#endif
#define SA_RLEN(sa) ((sa)->sa_len ? (((sa)->sa_len + SALIGN) & ~SALIGN) : (SALIGN + 1))

Ref<NetworkInterfaceList> NetworkInterface::queryAll(int family)
{
    Ref<NetworkInterfaceList> list = NetworkInterfaceList::create();

    int mib[6];
    mib[0] = CTL_NET;
    mib[1] = AF_ROUTE;
    mib[2] = 0 /*protocol*/;
    mib[3] = (family == -1) ? AF_UNSPEC : family;
    mib[4] = NET_RT_IFLIST;
    mib[5] = 0 /*flags*/;

    size_t bufSize = 0;
    if (::sysctl(mib, 6, NULL, &bufSize, NULL, 0) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    // race against changing address configuration
    char *buf = 0;
    while (true) {
        buf = (char *)cc::malloc(bufSize);
        if (!buf) CC_SYSTEM_DEBUG_ERROR(errno);

        if (::sysctl(mib, 6, (void *)buf, &bufSize, NULL, 0) == -1) {
            if (errno == ENOMEM) {
                cc::free(buf);
                bufSize *= 2; // arbitrary, but safe choice
                buf = (char *)cc::malloc(bufSize);
                continue;
            }
            CC_SYSTEM_DEBUG_ERROR(errno);
        }
        else
            break;
    }

    for (
        struct if_msghdr *msg = (struct if_msghdr *)buf;
        (char *)msg < buf + bufSize;
        msg = (struct if_msghdr *)((char *)msg + msg->ifm_msglen)
    )
    {
        int msgType = msg->ifm_type;
        int msgAddrs = msg->ifm_addrs;
        if (msgType == RTM_IFINFO) {
            Ref<NetworkInterface> interface = NetworkInterface::create();
            interface->index_ = msg->ifm_index;
            interface->flags_ = msg->ifm_flags;
            interface->mtu_ = msg->ifm_data.ifi_mtu;
            if (msgAddrs & RTA_IFP) {
                list->append(interface);
                struct sockaddr_dl *addr = (struct sockaddr_dl *)(msg + 1);
                if (addr->sdl_family == AF_LINK) { // paranoid check
                    interface->type_ = addr->sdl_type;
                    if (addr->sdl_nlen > 0)
                        interface->name_ = String(addr->sdl_data, addr->sdl_nlen);
                    if (addr->sdl_alen > 0)
                        interface->hardwareAddress_ = CharArray::copy(addr->sdl_data, addr->sdl_nlen);
                }
            }
        }
        else if ((msgType == RTM_NEWADDR) && (family != -1)) {
            struct ifa_msghdr *msga = (struct ifa_msghdr *)msg;
            char *attr = (char *)(msga + 1);
            CC_ASSERT(list->count() > 0);
            NetworkInterface *interface = list->at(list->count() - 1);
            // CC_ASSERT(interface->index_ == msga->ifam_index); // HACK, OpenBSD can fullfill
            Ref<SocketAddress> label;
            for (int i = 0; i < RTAX_MAX; ++i) {
                if (msga->ifam_addrs & (1 << i)) {
                    struct sockaddr *addr = (struct sockaddr *) attr;
                    Ref<SocketAddress> address;
                    int len = SA_RLEN(addr);
                    if (i == RTAX_IFA) {
                        if (addr->sa_family == AF_INET)
                            label = SocketAddressEntry::create((struct sockaddr_in *)addr);
                        else if (addr->sa_family == AF_INET6)
                            label = SocketAddress::create((struct sockaddr_in6 *)addr);
                        interface->addressList_->append(label);

                        if (addr->sa_family == AF_INET) {
                            SocketAddressEntry *entry = cast<SocketAddressEntry>(label);
                            if (i == RTAX_NETMASK) {
                                uint32_t x = ((struct sockaddr_in *)addr)->sin_addr.s_addr;
                                uint32_t y = ~(uint32_t)0;
                                int m = 32;
                                for(; m > 0; --m, y <<= 1)
                                    if (x == y) break;
                                entry->networkMask_ = m;
                            }
                            else if (i == RTAX_BRD) {
                                if (addr->sa_family == AF_INET)
                                    entry->broadcastAddress_ = SocketAddress::create((struct sockaddr_in *)addr);
                            }
                        }
                    }
                    attr += len;
                }
            }
        }
    }
    cc::free(buf);
    return list;
}
#endif

}} // namespace cc::net
