/*
 * Copyright (C) 2007-2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Map>
#include <cc/Set>
#include <cc/glob/Pattern>
#include "Notice.h"

namespace ccclaim {

using namespace cc;
using namespace cc::glob;

using Coverage = Map<String, Ref<Notice> >;

class Report: public Object
{
public:
    static Ref<Report> create(const StringList &dirPaths, Pattern works, int worksMinLines);

    StringList dirPaths() const { return dirPaths_; }
    Pattern works() const { return works_; }
    int worksMinLines() const { return worksMinLines_; }
    Coverage coverage() const { return coverage_; }
    Set<String> exposure() const { return exposure_; }
    Map<String, Coverage> coverageByDigest() const { return coverageByDigest_; }
    Map<String, Coverage> coverageByHolder() const { return coverageByHolder_; }
    Map<String, String> statementByDigest() const { return statementByDigest_; }

private:
    Report(const StringList &dirPaths, Pattern works, int worksMinLines);
    StringList dirPaths_;
    Pattern works_;
    int worksMinLines_;
    Coverage coverage_;
    Set<String> exposure_;
    Map<String, Coverage> coverageByDigest_;
    Map<String, Coverage> coverageByHolder_;
    Map<String, String> statementByDigest_;
};

} // namespace ccclaim
