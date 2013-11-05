#ifndef FCLAIM_REPORT_H
#define FCLAIM_REPORT_H

#include <flux/Pattern.h>
#include <flux/Map.h>
#include <flux/Set.h>
#include "Notice.h"

namespace fclaim
{

using namespace flux;

typedef Map<String, Ref<Notice> > Coverage;
typedef Set<String> Exposure;
typedef Map<String, Ref<Coverage> > CoverageByDigest;
typedef Map<String, Ref<Coverage> > CoverageByHolder;
typedef Map<String, String> StatementByDigest;

class Report: public Object
{
public:
	static Ref<Report> create(StringList *dirPaths, Pattern works);

	inline StringList *dirPaths() { return dirPaths_; }
	inline Pattern works() { return works_; }
	inline Coverage *coverage() { return coverage_; }
	inline Exposure *exposure() { return exposure_; }
	inline CoverageByDigest *coverageByDigest() { return coverageByDigest_; }
	inline CoverageByHolder *coverageByHolder() { return coverageByHolder_; }
	inline StatementByDigest *statementByDigest() { return statementByDigest_; }

private:
	Report(StringList *dirPaths, Pattern works);
	Ref<StringList> dirPaths_;
	Pattern works_;
	Ref<Coverage> coverage_;
	Ref<Exposure> exposure_;
	Ref<CoverageByDigest> coverageByDigest_;
	Ref<CoverageByHolder> coverageByHolder_;
	Ref<StatementByDigest> statementByDigest_;
};

} // namespace fclaim

#endif // FCLAIM_REPORT_H
