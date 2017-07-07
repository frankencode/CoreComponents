/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/meta/MetaObject>

namespace ccnode {

using namespace cc;
using namespace cc::meta;

class LogConfig: public Object
{
public:
    static Ref<LogConfig> loadDefault();
    static Ref<LogConfig> load(MetaObject *config);

    inline String path() const { return path_; }
    inline int level() const { return level_; }
    inline double retentionPeriod() const { return retentionPeriod_; }
    inline double rotationInterval() const { return rotationInterval_; }

private:
    LogConfig();
    LogConfig(MetaObject *config);

    String path_;
    int level_;
    double retentionPeriod_;
    double rotationInterval_;
};

} // namespace ccnode

