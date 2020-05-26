/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/string>
#include <cc/Map>
#include <cc/Singleton>

namespace ccbuild {

using namespace cc;

class ConfigureShell: public Object
{
public:
    static ConfigureShell *instance();
    void setVerbose(bool on) { verbose_ = on; }
    string run(const string &shellCommand);

private:
    friend class Singleton<ConfigureShell>;

    bool lookup(const string &command, string *output) const;
    void insert(const string &command, const string &output);

    ConfigureShell();

    bool verbose_;

    typedef Map<string, string> Cache;
    Ref<Cache> cache_;
};

} // namespace ccbuild
