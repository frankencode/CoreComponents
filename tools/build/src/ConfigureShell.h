/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/String>
#include <cc/MapValue>
#include <cc/Singleton>

namespace ccbuild {

using namespace cc;

class ConfigureShell: public Object
{
public:
    static ConfigureShell *instance();
    void setVerbose(bool on) { verbose_ = on; }
    String run(const String &shellCommand);

private:
    friend class Singleton<ConfigureShell>;

    bool lookup(const String &command, String *output) const;
    void insert(const String &command, const String &output);

    ConfigureShell();

    bool verbose_;

    MapValue<String> cache_;
};

} // namespace ccbuild
