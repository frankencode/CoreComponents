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

    void process(HttpRequest *request) override;
    bool process(HttpRequest *request, FileStatus *status, const String &documentRoot = "");
    void process(HttpRequest *request, const String &script, const String &documentRoot = "");

private:
    CgiDelegate(ServiceWorker *worker);

    typedef Map<String, String> EnvMap;

    Ref<EnvMap> makeEnv(HttpRequest *request, CharArray *payload) const;
    void logEnv(EnvMap *env);
    String compileHeader(HttpRequest *request, CharArray *payload) const;
    static String urlDecode(HttpRequest *request, CharArray *payload);
    static String wrapHttp(const String &header);

    Ref<const CgiInstance> cgiInstance_;
    int nextPeer_;
};

} // namespace ccnode
