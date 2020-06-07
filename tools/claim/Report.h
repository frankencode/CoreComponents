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

typedef Map<String, Ref<Notice> > Coverage;
typedef Set<String> Exposure;
typedef Map<String, Ref<Coverage> > CoverageByDigest;
typedef Map<String, Ref<Coverage> > CoverageByHolder;
typedef Map<String, String> StatementByDigest;

class Report: public Object
{
public:
    static Ref<Report> create(const StringList &dirPaths, Pattern works, int worksMinLines);

    StringList dirPaths() const { return dirPaths_; }
    Pattern works() const { return works_; }
    int worksMinLines() const { return worksMinLines_; }
    Coverage *coverage() const { return coverage_; }
    Exposure *exposure() const { return exposure_; }
    CoverageByDigest *coverageByDigest() const { return coverageByDigest_; }
    CoverageByHolder *coverageByHolder() const { return coverageByHolder_; }
    StatementByDigest *statementByDigest() const { return statementByDigest_; }

private:
    Report(const StringList &dirPaths, Pattern works, int worksMinLines);
    StringList dirPaths_;
    Pattern works_;
    int worksMinLines_;
    Ref<Coverage> coverage_;
    Ref<Exposure> exposure_;
    Ref<CoverageByDigest> coverageByDigest_;
    Ref<CoverageByHolder> coverageByHolder_;
    Ref<StatementByDigest> statementByDigest_;
};

} // namespace ccclaim
