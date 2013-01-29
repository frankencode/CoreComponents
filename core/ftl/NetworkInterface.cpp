 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
#include "File.hpp"
#include "LineSource.hpp"
#endif
#include "NetworkInterface.hpp"

namespace ftl
{

NetworkInterface::NetworkInterface()
	: index_(-1),
	  type_(0),
	  flags_(0),
	  hardwareAddress_(0),
	  mtu_(0)
{}

string NetworkInterface::name() const { return name_; }
int NetworkInterface::index() const { return index_; }
unsigned NetworkInterface::type() const { return type_; }
unsigned NetworkInterface::flags() const { return flags_; }
uint64_t NetworkInterface::hardwareAddress() const { return hardwareAddress_; }
uint32_t NetworkInterface::mtu() const { return mtu_; }
SocketAddressList *NetworkInterface::addressList() const { return addressList_; }

#ifdef __linux
hook<NetworkInterfaceList> NetworkInterface::queryAll(int family)
{
	hook<NetworkInterfaceList> list = NetworkInterfaceList::create();
	// getLink(list);
	for (int i = 1; getLink(list, i); ++i);

	int families[2];
	families[0] = ((family == AF_UNSPEC) || (family == AF_INET6)) ? AF_INET6 : -1;
	families[1] = ((family == AF_UNSPEC) || (family == AF_INET)) ? AF_INET : -1;

	for (int j = 0; j < 2; ++j)
	{
		if (families[j] == -1) continue;
		family = families[j];

		int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
		if (fd == -1) FTL_SYSTEM_EXCEPTION;
		int seq = 0;

		struct sockaddr_nl src;
		mem::clr(&src, sizeof(src));
		src.nl_family = AF_NETLINK;
		src.nl_pid = ::getpid();
		if (::bind(fd, (struct sockaddr *)&src, (socklen_t)sizeof(src)) == -1)
			FTL_SYSTEM_EXCEPTION;

		// send request
		{
			size_t msgLen = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
			struct nlmsghdr *msg = (struct nlmsghdr *)mem::alloc(msgLen);
			if (!msg) FTL_SYSTEM_EXCEPTION;

			mem::clr(msg, msgLen);
			msg->nlmsg_type = RTM_GETADDR;
			msg->nlmsg_len = msgLen;
			msg->nlmsg_flags = NLM_F_REQUEST|NLM_F_ROOT;
			msg->nlmsg_pid = src.nl_pid;
			msg->nlmsg_seq = seq++;

			struct ifaddrmsg *data = (struct ifaddrmsg *)NLMSG_DATA(msg);
			data->ifa_family = family;

			struct sockaddr_nl dst;
			mem::clr(&dst, sizeof(dst));
			dst.nl_family = AF_NETLINK;

			struct msghdr hdr;
			mem::clr(&hdr, sizeof(hdr));

			struct iovec iov = { msg, msgLen };
			hdr.msg_iov = &iov;
			hdr.msg_iovlen = 1;
			hdr.msg_name = (void *)&dst;
			hdr.msg_namelen = sizeof(dst);

			if (::sendmsg(fd, &hdr, 0/*flags*/) == -1)
				FTL_SYSTEM_EXCEPTION;

			mem::free(msg);
		}

		// process reply
		{
			struct msghdr hdr;
			mem::clr(&hdr, sizeof(hdr));

			ssize_t bufSize = ::recvmsg(fd, &hdr, MSG_PEEK|MSG_TRUNC);
			if (bufSize < 0) FTL_SYSTEM_EXCEPTION;

			void *buf = mem::alloc(bufSize);
			if (!buf) FTL_SYSTEM_EXCEPTION;

			struct iovec iov = { buf, (size_t)bufSize };
			hdr.msg_iov = &iov;
			hdr.msg_iovlen = 1;
			ssize_t bufFill = ::recvmsg(fd, &hdr, 0/*flags*/);

			if (::close(fd) == -1)
				FTL_SYSTEM_EXCEPTION;

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

					hook<SocketAddress> label;
					NetworkInterface *interface = 0;

					for (int i = 0; i < list->length(); ++i) {
						if (unsigned(list->at(i)->index_) == data->ifa_index) {
							interface = list->at(i);
							break;
						}
					}

					if (!interface) {
						if (!getLink(list, data->ifa_index)) continue;
						interface = list->at(list->length() - 1);
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
							hook<SocketAddress> address;
							struct sockaddr_in addr4;
							struct sockaddr_in6 addr6;
							if (data->ifa_family == AF_INET) {
								mem::clr(&addr4, sizeof(addr4));
								// addr.sin_len = sizeof(addr);
								*(uint8_t *)&addr4 = sizeof(addr4); // uggly, but safe HACK, for BSD4.4
								addr4.sin_family = AF_INET;
								addr4.sin_addr = *(struct in_addr *)RTA_DATA(attr);
								if (attrType != IFA_ADDRESS)
									address = SocketAddress::create(&addr4);
							}
							else if (data->ifa_family == AF_INET6) {
								mem::clr(&addr6, sizeof(addr6));
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
									label = SocketAddressEntry::create(&addr4);
								else if (data->ifa_family == AF_INET6)
									label = SocketAddress::create(&addr6);
								if (!interface->addressList_) interface->addressList_ = SocketAddressList::create();
								interface->addressList_->append(label);
							}
							if ((label) && (data->ifa_family == AF_INET)) {
								SocketAddressEntry *entry = cast<SocketAddressEntry>(label.get());
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

			mem::free(buf);
		}
	}

	if (list->length() == 0)
		list = queryAllIoctl(family);

	return list;
}

bool NetworkInterface::getLink(NetworkInterfaceList *list, int index)
{
	bool foundSomething = false;

	{
		int fd = ::socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
		if (fd == -1) FTL_SYSTEM_EXCEPTION;
		int seq = 0;

		struct sockaddr_nl src;
		mem::clr(&src, sizeof(src));
		src.nl_family = AF_NETLINK;
		src.nl_pid = ::getpid();
		if (::bind(fd, (struct sockaddr *)&src, (socklen_t)sizeof(src)) == -1)
			FTL_SYSTEM_EXCEPTION;

		// send request
		{
			size_t msgLen = NLMSG_LENGTH(sizeof(struct ifinfomsg));
			struct nlmsghdr *msg = (struct nlmsghdr *)mem::alloc(msgLen);
			if (!msg) FTL_SYSTEM_EXCEPTION;

			mem::clr(msg, msgLen);
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
			mem::clr(&dst, sizeof(dst));
			dst.nl_family = AF_NETLINK;

			struct msghdr hdr;
			mem::clr(&hdr, sizeof(hdr));

			struct iovec iov = { msg, msgLen };
			hdr.msg_iov = &iov;
			hdr.msg_iovlen = 1;
			hdr.msg_name = (void *)&dst;
			hdr.msg_namelen = sizeof(dst);

			if (::sendmsg(fd, &hdr, 0/*flags*/) == -1)
				FTL_SYSTEM_EXCEPTION;

			mem::free(msg);
		}

		// process reply
		{
			struct msghdr hdr;
			mem::clr(&hdr, sizeof(hdr));

			ssize_t bufSize = ::recvmsg(fd, &hdr, MSG_PEEK|MSG_TRUNC);
			if (bufSize < 0) FTL_SYSTEM_EXCEPTION;

			void *buf = mem::alloc(bufSize);
			if (!buf) FTL_SYSTEM_EXCEPTION;

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

					hook<NetworkInterface> interface = NetworkInterface::create();
					foundSomething = true;
					list->append(interface);
					interface->index_ = data->ifi_index;
					interface->type_ = data->ifi_type;
					interface->flags_ = data->ifi_flags;

					for (;RTA_OK(attr,attrFill); attr = RTA_NEXT(attr, attrFill))
					{
						unsigned attrType = attr->rta_type;
						unsigned attrLen = RTA_PAYLOAD(attr);

						if ((attrType == IFLA_ADDRESS) || (attrType == IFLA_BROADCAST)) {
							// strange fact: hardware address always stored in little endian
							unsigned char *value = (unsigned char *)RTA_DATA(attr);
							uint64_t h = 0;
							for (unsigned i = 0; i < attrLen; ++i) {
								h <<= 8;
								h |= value[i];
							}

							if (attrType == IFLA_ADDRESS)
								interface->hardwareAddress_= h;
							//else if (attrType == IFLA_BROADCAST)
							//	interface->broadcastAddress_ = h;
						}
						else if (attrType == IFLA_IFNAME) {
							interface->name_ = (char *)RTA_DATA(attr);
						}
						else if (attrType == IFLA_MTU) {
							if (attrLen == 4)
								interface->mtu_ = *(uint32_t *)RTA_DATA(attr);
						}
					}
				}
			}

			mem::free(buf);
		}
		if (::close(fd) == -1)
			FTL_SYSTEM_EXCEPTION;
	}
	return foundSomething;
}

hook<NetworkInterfaceList> NetworkInterface::queryAllIoctl(int family)
{
	hook<NetworkInterfaceList> list = NetworkInterfaceList::create();

	int fd = ::socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) FTL_SYSTEM_EXCEPTION;

	hook<LineSource> source = LineSource::open(File::open("/proc/net/dev", File::Read));
	for (string line; source->read(&line);) {
		if (line->contains(':')) {
			hook<NetworkInterface> interface = NetworkInterface::create();
			list->append(interface);
			hook<StringList> parts = line->split(":");
			string name = parts->at(0)->stripLeadingSpace();
			interface->name_ = name;

			{
				struct ifreq ifr;
				mem::clr(&ifr, sizeof(ifr));
				for (int i = 0, n = name->length(); i < n; ++i)
					ifr.ifr_name[i] = name->get(i);

				if (::ioctl(fd, SIOCGIFHWADDR, &ifr) == -1)
					FTL_SYSTEM_EXCEPTION;
				interface->hardwareAddress_ = 0;
				for (int i = 0, n = 6; i < n; ++i) // quick HACK, 6 is just a safe bet
					interface->hardwareAddress_ = (interface->hardwareAddress_ << 8) | ((unsigned char *)ifr.ifr_hwaddr.sa_data)[i];
				if (::ioctl(fd, SIOCGIFFLAGS, &ifr) == -1)
					FTL_SYSTEM_EXCEPTION;
				interface->flags_ = ifr.ifr_flags;

				if (::ioctl(fd, SIOCGIFMTU, &ifr) == -1)
					FTL_SYSTEM_EXCEPTION;
				interface->mtu_ = ifr.ifr_mtu;
			}
		}
	}

	{
		struct ifconf ifc;
		int capa = 32 * sizeof(struct ifreq);
		ifc.ifc_len = capa;
		ifc.ifc_req = (struct ifreq *)mem::alloc(capa);

		while (true) {
			if (::ioctl(fd, SIOCGIFCONF, &ifc) == -1)
				FTL_SYSTEM_EXCEPTION;
			if (ifc.ifc_len == capa) {
				mem::free(ifc.ifc_req);
				capa *= 2;
				ifc.ifc_len = capa;
				ifc.ifc_req = (struct ifreq *)mem::alloc(capa);
				continue;
			}
			break;
		}

		for (int i = 0; i < int(ifc.ifc_len / sizeof(struct ifreq)); ++i)
		{
			struct ifreq *ifr = ifc.ifc_req + i;

			if ((family != AF_UNSPEC) && (family != ifr->ifr_addr.sa_family))
				continue;

			for (int k = 0; k < list->length(); ++k)
			{
				NetworkInterface *interface = list->at(k);

				if (interface->name_ == ifr->ifr_name) {
					struct sockaddr *addr = &ifr->ifr_addr;
					struct sockaddr_in *addr4 = (struct sockaddr_in *)addr;
					struct sockaddr_in6 *addr6 = (struct sockaddr_in6 *)addr;

					hook<SocketAddress> label;

					if (addr->sa_family == AF_INET)
						label = SocketAddressEntry::create(addr4);
					else if (addr->sa_family == AF_INET6)
						label = SocketAddress::create(addr6);
					if (!interface->addressList_) interface->addressList_ = SocketAddressList::create();
					interface->addressList_->append(label);

					if ((label) && (addr->sa_family == AF_INET)) {
						SocketAddressEntry *entry = cast<SocketAddressEntry>(label.get());
						if (interface->flags_ & IFF_BROADCAST) {
							struct ifreq ifr2 = *ifr;
							if (::ioctl(fd, SIOCGIFBRDADDR, &ifr2) == -1)
								FTL_SYSTEM_EXCEPTION;
							entry->broadcastAddress_ = SocketAddress::create((struct sockaddr_in *)&ifr2.ifr_broadaddr);
						}
						if (interface->flags_ & IFF_POINTOPOINT) {
							struct ifreq ifr2 = *ifr;
							if (::ioctl(fd, SIOCGIFDSTADDR, &ifr2) == -1)
								FTL_SYSTEM_EXCEPTION;
							entry->broadcastAddress_ = SocketAddress::create((struct sockaddr_in *)&ifr2.ifr_dstaddr);
						}
					}
				}
			}
		}

		mem::free(ifc.ifc_req);
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

hook<NetworkInterfaceList> NetworkInterface::queryAll(int family)
{
	hook<NetworkInterfaceList> list = new NetworkInterfaceList;

	int mib[6];
	mib[0] = CTL_NET;
	mib[1] = AF_ROUTE;
	mib[2] = 0 /*protocol*/;
	mib[3] = (family == -1) ? AF_UNSPEC : family;
	mib[4] = NET_RT_IFLIST;
	mib[5] = 0 /*flags*/;

	size_t bufSize = 0;
	if (::sysctl(mib, 6, NULL, &bufSize, NULL, 0) == -1)
		FTL_SYSTEM_EXCEPTION;

	// race against changing address configuration
	char *buf = 0;
	while (true) {
		buf = (char *)mem::alloc(bufSize);
		if (!buf) FTL_SYSTEM_EXCEPTION;

		if (::sysctl(mib, 6, (void *)buf, &bufSize, NULL, 0) == -1) {
			if (errno == ENOMEM) {
				mem::free(buf);
				bufSize *= 2; // arbitrary, but safe choice
				buf = (char *)mem::alloc(bufSize);
				continue;
			}
			FTL_SYSTEM_EXCEPTION;
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
			hook<NetworkInterface> interface = NetworkInterface::create();
			interface->index_ = msg->ifm_index;
			interface->flags_ = msg->ifm_flags;
			interface->mtu_ = msg->ifm_data.ifi_mtu;
			if (msgAddrs & RTA_IFP) {
				list->append(interface);
				struct sockaddr_dl *addr = (struct sockaddr_dl *)(msg + 1);
				if (addr->sdl_family == AF_LINK) { // paranoid check
					interface->type_ = addr->sdl_type;
					if (addr->sdl_nlen > 0)
						interface->name_ = string(addr->sdl_data, addr->sdl_nlen);
					if (addr->sdl_alen > 0) {
						// strange fact: hardware address always stored in little endian
						unsigned char *value = (unsigned char *)addr->sdl_data + addr->sdl_nlen;
						uint64_t h = 0;
						for (int i = 0, n = addr->sdl_alen; i < n; ++i) {
							h <<= 8;
							h |= value[i];
						}
						interface->hardwareAddress_ = h;
					}
				}
			}
		}
		else if ((msgType == RTM_NEWADDR) && (family != -1)) {
			struct ifa_msghdr *msga = (struct ifa_msghdr *)msg;
			char *attr = (char *)(msga + 1);
			FTL_ASSERT(list->length() > 0);
			NetworkInterface *interface = list->at(-1);
			// FTL_ASSERT(interface->index_ == msga->ifam_index); // HACK, OpenBSD can fullfill
			hook<SocketAddress> label;
			for (int i = 0; i < RTAX_MAX; ++i) {
				if (msga->ifam_addrs & (1 << i)) {
					struct sockaddr *addr = (struct sockaddr *) attr;
					hook<SocketAddress> address;
					int len = SA_RLEN(addr);
					if (i == RTAX_IFA) {
						if (addr->sa_family == AF_INET)
							label = SocketAddressEntry::create((struct sockaddr_in *)addr);
						else if (addr->sa_family == AF_INET6)
							label = SocketAddress::create((struct sockaddr_in6 *)addr);
						if (!interface->addressList_) interface->addressList_ = new SocketAddressList;
						interface->addressList_->append(label);
					}
					if (addr->sa_family == AF_INET) {

						SocketAddressEntry *entry = cast<SocketAddressEntry>(label);
						/*/if (i == RTAX_NETMASK) {
								label->netmask_ = netmask;
						}
						else*/
						if (i == RTAX_BRD) {
							if (addr->sa_family == AF_INET)
								entry->broadcastAddress_ = SocketAddress::create((struct sockaddr_in *)addr);
						}
					}
					attr += len;
				}
			}
		}
	}
	mem::free(buf);
	return list;
}
#endif

} // namespace ftl
