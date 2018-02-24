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
#include <cc/regexp/RegExp>
#include "Notice.h"

namespace ccclaim {

using namespace cc;
using namespace cc::regexp;

typedef Map<String, Ref<Notice> > Coverage;
typedef Set<String> Exposure;
typedef Map<String, Ref<Coverage> > CoverageByDigest;
typedef Map<String, Ref<Coverage> > CoverageByHolder;
typedef Map<String, String> StatementByDigest;

class Report: public Object
{
public:
    static Ref<Report> create(const StringList *dirPaths, RegExp works, int worksMinLines);

    inline const StringList *dirPaths() const { return dirPaths_; }
    inline RegExp works() const { return works_; }
    inline int worksMinLines() const { return worksMinLines_; }
    inline Coverage *coverage() const { return coverage_; }
    inline Exposure *exposure() const { return exposure_; }
    inline CoverageByDigest *coverageByDigest() const { return coverageByDigest_; }
    inline CoverageByHolder *coverageByHolder() const { return coverageByHolder_; }
    inline StatementByDigest *statementByDigest() const { return statementByDigest_; }

private:
    Report(const StringList *dirPaths, RegExp works, int worksMinLines);
    Ref<const StringList> dirPaths_;
    RegExp works_;
    int worksMinLines_;
    Ref<Coverage> coverage_;
    Ref<Exposure> exposure_;
    Ref<CoverageByDigest> coverageByDigest_;
    Ref<CoverageByHolder> coverageByHolder_;
    Ref<StatementByDigest> statementByDigest_;
};

} // namespace ccclaim
