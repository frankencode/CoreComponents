/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ServiceDelegate.h"

namespace fluxnode
{

class EchoDelegate: public ServiceDelegate
{
public:
	static Ref<EchoDelegate> create(ServiceWorker *worker) {
		return new EchoDelegate(worker);
	}

	virtual void process(Request *request)
	{
		{
			Format echo = chunk();
			echo << request->method() << " " << request->target() << " " << request->version() << "\r\n";
			for (int i = 0; i < request->count(); ++i)
				echo << request->keyAt(i) << ": " << request->valueAt(i) << "\r\n";
			echo << "\r\n";
		}
		Ref<ByteArray> buf = ByteArray::create(0x3FFF);
		while (true) {
			int n = request->payload()->read(buf);
			if (n == 0) break;
			write(ByteRange(buf, 0, n));
		}
	}

private:
	EchoDelegate(ServiceWorker *worker)
		: ServiceDelegate(worker)
	{}
};

} // namespace fluxnode
