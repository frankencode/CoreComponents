/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/check>
#include <flux/net/Uri>

using namespace flux;
using namespace flux::net;

int main()
{
    Ref<Uri> uri = Uri::create("http://john@example.com:8000/%7ejohn/123.php?say=hello#part1");
    fout("uri->scheme() = \"%%\"\n") << uri->scheme();
    fout("uri->userInfo() = \"%%\"\n") << uri->userInfo();
    fout("uri->host() = \"%%\"\n") << uri->host();
    fout("uri->port() = %%\n") << uri->port();
    fout("uri->path() = \"%%\"\n") << uri->path();
    fout("uri->query() = \"%%\"\n") << uri->query();
    fout("uri->fragment() = \"%%\"\n") << uri->fragment();
    check(uri->scheme() == "http");
    check(uri->userInfo() == "john");
    check(uri->host() == "example.com");
    check(uri->port() == 8000);
    check(uri->path() == "/~john/123.php");
    check(uri->query() == "say=hello");
    check(uri->fragment() == "part1");
    return 0;
}
