/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/TransferLimiter.h>
#include <fkit/LineSource.h>
#include <fkit/System.h>
#include "exceptions.h"
#include "ClientConnection.h"
#include "Request.h"

namespace fnode
{

Ref<Request> Request::parse(ClientConnection *client, int maxHeaderSize)
{
	return new Request(client, maxHeaderSize);
}

Request::Request(ClientConnection *client, int maxHeaderSize)
	: requestTime_(System::now())
{
	try {
		Ref<TransferLimiter> limiter = TransferLimiter::open(client->stream(), maxHeaderSize);
		Ref<ByteArray> buf = ByteArray::create(4093);
		Ref<LineSource> source = LineSource::open(limiter, buf);

		String line;

		if (!source->read(&line)) throw BadRequest();
		requestLine_ = line;

		if (line->count(' ') != 2) throw BadRequest();
		{
			int i0 = 0, i1 = line->find(' ');
			method_ = line->copy(i0, i1);
			i0 = i1 + 1; i1 = line->find(' ', i0);
			target_ = line->copy(i0, i1);
			version_ = line->copy(i1 + 1, line->size());
		}

		if (version_ != "HTTP/1.1") throw UnsupportedVersion();

		String name, value;
		Ref<StringList> multiValue;
		while (source->read(&line)) {
			if (line == "")
				break;
			if (line->at(0) == ' ' || line->at(0) == '\t') {
				if (!multiValue) {
					multiValue = StringList::create();
					multiValue->append(value);
				}
				multiValue->append(line->trimInsitu());
				continue;
			}
			if (multiValue) {
				establish(name, multiValue->join());
				multiValue = 0;
			}
			int i = line->find(':');
			if (i == line->size()) throw BadRequest();
			name = line->copy(0, i)->trimInsitu();
			value = line->copy(i + 1, line->size())->trimInsitu();
			establish(name, value);
		}
		if (multiValue)
			establish(name, multiValue->join());
		client->pendingData_ = source->pendingData();
	}
	catch (ReadLimitExceeded &) {
		throw BadRequest();
	}

	host_ = value("Host");
	if (host_ == "") throw BadRequest();
}

} // namespace fnode
