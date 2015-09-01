/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXCLAIM_REPORT_H
#define FLUXCLAIM_REPORT_H

#include <flux/Map>
#include <flux/Set>
#include <flux/syntax/RegExp>
#include "Notice.h"

namespace fluxclaim {

using namespace flux;

typedef Map<String, Ref<Notice> > Coverage;
typedef Set<String> Exposure;
typedef Map<String, Ref<Coverage> > CoverageByDigest;
typedef Map<String, Ref<Coverage> > CoverageByHolder;
typedef Map<String, String> StatementByDigest;

class Report: public Object
{
public:
    static Ref<Report> create(StringList *dirPaths, RegExp works, int worksMinLines);

    inline StringList *dirPaths() const { return dirPaths_; }
    inline RegExp works() const { return works_; }
    inline int worksMinLines() const { return worksMinLines_; }
    inline Coverage *coverage() const { return coverage_; }
    inline Exposure *exposure() const { return exposure_; }
    inline CoverageByDigest *coverageByDigest() const { return coverageByDigest_; }
    inline CoverageByHolder *coverageByHolder() const { return coverageByHolder_; }
    inline StatementByDigest *statementByDigest() const { return statementByDigest_; }

private:
    Report(StringList *dirPaths, RegExp works, int worksMinLines);
    Ref<StringList> dirPaths_;
    RegExp works_;
    int worksMinLines_;
    Ref<Coverage> coverage_;
    Ref<Exposure> exposure_;
    Ref<CoverageByDigest> coverageByDigest_;
    Ref<CoverageByHolder> coverageByHolder_;
    Ref<StatementByDigest> statementByDigest_;
};

} // namespace fluxclaim

#endif // FLUXCLAIM_REPORT_H
