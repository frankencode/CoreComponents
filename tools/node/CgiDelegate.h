/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/FileStatus>
#include "ServiceDelegate.h"

namespace ccnode {

class CgiInstance;

class CgiDelegate: public ServiceDelegate
{
public:
    static Ref<CgiDelegate> create(ServiceWorker *worker);

    virtual void process(HttpRequest *request);
    bool process(HttpRequest *request, FileStatus *status, String documentRoot = "");
    void process(HttpRequest *request, String script, String documentRoot = "");

private:
    CgiDelegate(ServiceWorker *worker);

    typedef Map<String, String> EnvMap;

    Ref<EnvMap> makeEnv(HttpRequest *request, CharArray *payload) const;
    void logEnv(EnvMap *env);
    String compileHeader(HttpRequest *request, CharArray *payload) const;
    static String urlDecode(HttpRequest *request, CharArray *payload);
    static String wrapHttp(String header);

    Ref<CgiInstance> cgiInstance_;
    int nextPeer_;
};

} // namespace ccnode
