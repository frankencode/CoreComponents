/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "ServiceDelegate.h"

namespace fnode
{

class EchoDelegate: public ServiceDelegate
{
public:
	static Ref<EchoDelegate> create(ServiceWorker *worker) {
		return new EchoDelegate(worker);
	}

	virtual void process(Request *request)
	{
		if (request->method() != "GET") return;
		Format echo = chunk();
		echo << request->method() << " " << request->target() << " " << request->version() << "\r\n";
		for (int i = 0; i < request->size(); ++i)
			echo << request->keyAt(i) << ": " << request->valueAt(i) << "\r\n";
	}

private:
	EchoDelegate(ServiceWorker *worker)
		: ServiceDelegate(worker)
	{}
};

} // namespace fnode
