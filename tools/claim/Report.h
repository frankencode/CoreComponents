/*
 * Copyright (C) 2007-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/MapValue>
#include <cc/SetValue>
#include <cc/glob/Pattern>
#include "Notice.h"

namespace ccclaim {

using namespace cc;
using namespace cc::glob;

using Coverage = MapValue<String, Ref<Notice> >;

class Report: public Object
{
public:
    static Ref<Report> create(const StringList &dirPaths, Pattern works, int worksMinLines);

    StringList dirPaths() const { return dirPaths_; }
    Pattern works() const { return works_; }
    int worksMinLines() const { return worksMinLines_; }
    Coverage coverage() const { return coverage_; }
    SetValue<String> exposure() const { return exposure_; }
    MapValue<String, Coverage> coverageByDigest() const { return coverageByDigest_; }
    MapValue<String, Coverage> coverageByHolder() const { return coverageByHolder_; }
    MapValue<String, String> statementByDigest() const { return statementByDigest_; }

private:
    Report(const StringList &dirPaths, Pattern works, int worksMinLines);
    StringList dirPaths_;
    Pattern works_;
    int worksMinLines_;
    Coverage coverage_;
    SetValue<String> exposure_;
    MapValue<String, Coverage> coverageByDigest_;
    MapValue<String, Coverage> coverageByHolder_;
    MapValue<String, String> statementByDigest_;
};

} // namespace ccclaim
