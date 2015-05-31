/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifdef __linux
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h> // socket, sendmsg, recvmsg
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <errno.h>
#endif // __linux

#include <flux/exceptions>
#ifdef __linux
#include <flux/Map>
#include <flux/net/NetworkInterface>
#include <flux/stdio> // DEBUG
#endif // __Linux
#include <flux/net/RouteInfo>

namespace flux {
namespace net {

#ifdef __linux

Ref<RouteInfoList> RouteInfo::queryTable()
{
    Ref<RouteInfoList> list = RouteInfoList::create();

    int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd == -1) FLUX_SYSTEM_DEBUG_ERROR(errno);

    struct sockaddr_nl src;
    memclr(&src, sizeof(src));
    src.nl_family = AF_NETLINK;
    src.nl_pid = ::getpid();
    if (::bind(fd, (struct sockaddr *)&src, (socklen_t)sizeof(src)) == -1)
        FLUX_SYSTEM_DEBUG_ERROR(errno);

    // send request
    {
        size_t msgLen = NLMSG_LENGTH(sizeof(struct rtmsg));
        struct nlmsghdr *msg = (struct nlmsghdr *)flux::malloc(msgLen);
        int seq = 0;

        memclr(msg, msgLen);
        msg->nlmsg_type = RTM_GETROUTE;
        msg->nlmsg_len = msgLen;
        msg->nlmsg_flags = NLM_F_REQUEST|NLM_F_DUMP;
        msg->nlmsg_pid = src.nl_pid;
        msg->nlmsg_seq = seq++;

        struct rtmsg *data = (struct rtmsg *)NLMSG_DATA(msg);
        data->rtm_table = RT_TABLE_DEFAULT;
        data->rtm_protocol = RTPROT_UNSPEC;

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
            FLUX_SYSTEM_DEBUG_ERROR(errno);

        flux::free(msg);
    }

    typedef Map<int, Ref<NetworkInterface> > InterfaceByIndex;
    Ref<InterfaceByIndex> interfaceByIndex = InterfaceByIndex::create();

    // process reply
    {
        struct msghdr hdr;
        memclr(&hdr, sizeof(hdr));

        ssize_t bufSize = ::recvmsg(fd, &hdr, MSG_PEEK|MSG_TRUNC);
        if (bufSize < 0) FLUX_SYSTEM_DEBUG_ERROR(errno);

        void *buf = flux::malloc(bufSize);
        if (!buf) FLUX_SYSTEM_DEBUG_ERROR(errno);

        struct iovec iov = { buf, (size_t)bufSize };
        hdr.msg_iov = &iov;
        hdr.msg_iovlen = 1;
        ssize_t bufFill = ::recvmsg(fd, &hdr, 0/*flags*/);

        if (::close(fd) == -1)
            FLUX_SYSTEM_DEBUG_ERROR(errno);

        struct nlmsghdr *msg = (struct nlmsghdr *)buf;

        for (;NLMSG_OK(msg, unsigned(bufFill)); msg = NLMSG_NEXT(msg, bufFill))
        {
            unsigned msgType = msg->nlmsg_type;

            if (msgType == RTM_NEWROUTE) {
                struct rtmsg *data = (struct rtmsg *)NLMSG_DATA(msg);
                struct rtattr *attr = (struct rtattr *)RTM_RTA(data);
                int attrFill = NLMSG_PAYLOAD(msg, sizeof(struct rtmsg));

                Ref<RouteInfo> info = RouteInfo::create();
                list->append(info);
                info->sourceMask_ = data->rtm_src_len;
                info->destinationMask_ = data->rtm_dst_len;

                for (;RTA_OK(attr, attrFill); attr = RTA_NEXT(attr, attrFill))
                {
                    unsigned attrType = attr->rta_type;

                    if (
                        attrType == RTA_DST ||
                        attrType == RTA_SRC ||
                        attrType == RTA_GATEWAY
                    ) {
                        Ref<SocketAddress> address;
                        struct sockaddr_in addr4;
                        struct sockaddr_in6 addr6;
                        if (data->rtm_family == AF_INET) {
                            memclr(&addr4, sizeof(addr4));
                            // addr.sin_len = sizeof(addr);
                            *(uint8_t *)&addr4 = sizeof(addr4); // uggly, but safe HACK, for BSD4.4
                            addr4.sin_family = AF_INET;
                            addr4.sin_addr = *(struct in_addr *)RTA_DATA(attr);
                            address = SocketAddress::create(&addr4);
                        }
                        else if (data->rtm_family == AF_INET6) {
                            memclr(&addr6, sizeof(addr6));
                            #ifdef SIN6_LEN
                            addr.sin6_len = sizeof(addr6);
                            #endif
                            addr6.sin6_family = AF_INET6;
                            addr6.sin6_addr = *(struct in6_addr *)RTA_DATA(attr);
                            // addr6.sin6_scope_id = data->rtm_scope;
                            address = SocketAddress::create(&addr6);
                        }
                        if (attrType == RTA_DST)
                            info->destination_ = address;
                        else if (attrType == RTA_SRC)
                            info->source_ = address;
                        else if (attrType == RTA_GATEWAY)
                            info->gateway_ = address;
                    }
                    else if (
                        attrType == RTA_IIF ||
                        attrType == RTA_OIF
                    ) {
                        int index = *(int *)RTA_DATA(attr);
                        Ref<NetworkInterface> interface;
                        if (!interfaceByIndex->lookup(index, &interface)) {
                            interface = NetworkInterface::getLink(index);
                            interfaceByIndex->insert(index, interface);
                        }
                        if (attrType == RTA_IIF)
                            info->inputInterface_ = interface->name();
                        else
                            info->outputInterface_ = interface->name();
                    }
                    else if (attrType == RTA_PRIORITY) {
                        info->priority_ = *(int *)RTA_DATA(attr);
                    }
                    else if (attrType == RTA_METRICS) {
                        info->metric_ =  *(int *)RTA_DATA(attr);
                    }
                }
            }
            else if (msgType == NLMSG_DONE) { // paranoid HACK
                break;
            }
        }

        flux::free(buf);
    }

    return list;
}

#else // __linux

Ref<RouteInfoList> RouteInfo::queryTable()
{
    #warning "RouteInfo is currently not supported for non-Linux systems"
    return RouteInfoList::create();
}

#endif // __linux

}} // namespace flux::net
