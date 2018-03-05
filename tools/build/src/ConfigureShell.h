/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/Map>
#include <cc/Singleton>

namespace ccbuild {

using namespace cc;

class ConfigureShell: public Object
{
public:
    static ConfigureShell *instance();
    void setVerbose(bool on) { verbose_ = on; }
    String run(String shellCommand);

private:
    friend class Singleton<ConfigureShell>;

    bool lookup(String command, String *output) const;
    void insert(String command, String output);

    ConfigureShell();

    bool verbose_;

    typedef Map<String, String> Cache;
    Ref<Cache> cache_;
};

} // namespace ccbuild
