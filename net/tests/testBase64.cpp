/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/check>
#include <flux/net/base64>

using namespace flux;
using namespace flux::net;

int main()
{
	String test[] = { "Man", "Hello world", "", "1" };
	const int testCount = sizeof(test) / sizeof(test[0]);

	for (int i = 0; i < testCount; ++i) {
		String a = test[i], b = base64::encode(a);
		fout("base64(\"%%\") = \"%%\"\n") << a << b;
		check(base64::decode(b) == a);
	}

	return 0;
}
