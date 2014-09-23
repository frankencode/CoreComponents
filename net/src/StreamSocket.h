/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNET_STREAMSOCKET_H
#define FLUXNET_STREAMSOCKET_H

#include <flux/SystemStream.h>
#include "SocketAddress.h"

namespace flux {
namespace net {

class StreamSocket: public SystemStream
{
public:
	static Ref<StreamSocket> listen(SocketAddress *address);
	static Ref<StreamSocket> connect(SocketAddress *address);

	SocketAddress *address() const;
	bool getPeerAddress(SocketAddress *address);

	bool readyAccept(double interval);
	Ref<StreamSocket> accept();
	bool established(double interval);
	void shutdown(int how = SHUT_RDWR);

	void setRecvTimeout(double interval);
	void setSendTimeout(double interval);

protected:
	StreamSocket(SocketAddress *address);
	StreamSocket(SocketAddress *address, int fdc);
	void bind();
	void listen(int backlog = 8);
	void connect();

	Ref<SocketAddress> address_;
	bool connected_;
};

}} // namespace flux::net

#endif // FLUXNET_STREAMSOCKET_H
