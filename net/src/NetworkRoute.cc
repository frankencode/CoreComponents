/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/NetworkRoute>
#ifdef __linux
#include <cc/NetworkInterface>
#include <cc/SystemError>
#include <cc/Map>
#include <sys/types.h>
#include <sys/socket.h> // socket, sendmsg, recvmsg
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#endif // __linux

namespace cc {

List<NetworkRoute> NetworkRoute::queryAll()
{
    List<NetworkRoute> list;

    int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd == -1) CC_SYSTEM_DEBUG_ERROR(errno);

    struct sockaddr_nl src;
    std::memset(&src, 0, sizeof(src));
    src.nl_family = AF_NETLINK;
    src.nl_pid = ::getpid();
    if (::bind(fd, (struct sockaddr *)&src, (socklen_t)sizeof(src)) == -1)
        CC_SYSTEM_DEBUG_ERROR(errno);

    // send request
    {
        size_t msgLen = NLMSG_LENGTH(sizeof(struct rtmsg));
        Bytes buf = Bytes::allocate(static_cast<long>(msgLen));
        struct nlmsghdr *msg = (struct nlmsghdr *)buf.items();
        int seq = 0;

        std::memset(msg, 0, msgLen);
        msg->nlmsg_type = RTM_GETROUTE;
        msg->nlmsg_len = msgLen;
        msg->nlmsg_flags = NLM_F_REQUEST|NLM_F_DUMP;
        msg->nlmsg_pid = src.nl_pid;
        msg->nlmsg_seq = seq++;

        struct rtmsg *data = (struct rtmsg *)NLMSG_DATA(msg);
        data->rtm_table = RT_TABLE_DEFAULT;
        data->rtm_protocol = RTPROT_UNSPEC;

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

    Map<int, NetworkInterface> interfaceByIndex;

    // process reply
    {
        struct msghdr hdr;
        std::memset(&hdr, 0, sizeof(hdr));

        ssize_t msgLen = ::recvmsg(fd, &hdr, MSG_PEEK|MSG_TRUNC);
        if (msgLen < 0) CC_SYSTEM_DEBUG_ERROR(errno);

        Bytes buf = Bytes::allocate(msgLen);
        if (!buf) CC_SYSTEM_DEBUG_ERROR(errno);

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

            if (msgType == RTM_NEWROUTE) {
                struct rtmsg *data = (struct rtmsg *)NLMSG_DATA(msg);
                struct rtattr *attr = (struct rtattr *)RTM_RTA(data);
                int attrFill = NLMSG_PAYLOAD(msg, sizeof(struct rtmsg));

                State *info = new State;
                list.append(NetworkRoute{info});

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
                        SocketAddress address;
                        struct sockaddr_in addr4;
                        struct sockaddr_in6 addr6;
                        if (data->rtm_family == AF_INET) {
                            std::memset(&addr4, 0, sizeof(addr4));
                            // addr.sin_len = sizeof(addr);
                            *(uint8_t *)&addr4 = sizeof(addr4); // uggly, but safe HACK, for BSD4.4
                            addr4.sin_family = AF_INET;
                            addr4.sin_addr = *(struct in_addr *)RTA_DATA(attr);
                            address = SocketAddress{&addr4};
                        }
                        else if (data->rtm_family == AF_INET6) {
                            std::memset(&addr6, 0, sizeof(addr6));
                            #ifdef SIN6_LEN
                            addr.sin6_len = sizeof(addr6);
                            #endif
                            addr6.sin6_family = AF_INET6;
                            addr6.sin6_addr = *(struct in6_addr *)RTA_DATA(attr);
                            // addr6.sin6_scope_id = data->rtm_scope;
                            address = SocketAddress{&addr6};
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
                        NetworkInterface interface;
                        if (!interfaceByIndex.lookup(index, &interface)) {
                            interface = NetworkInterface::getLink(index);
                            interfaceByIndex.insert(index, interface);
                        }
                        if (attrType == RTA_IIF)
                            info->inputInterface_ = interface.name();
                        else
                            info->outputInterface_ = interface.name();
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
    }

    return list;
}

} // namespace cc
