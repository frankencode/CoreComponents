#ifndef FCLAIM_COPYRIGHT_H
#define FCLAIM_COPYRIGHT_H

#include <fkit/String.h>
#include <fkit/List.h>

namespace fclaim
{

using namespace fkit;

class Copyright: public Object
{
public:
	inline static Ref<Copyright> create(String holder, int yearStart, int yearEnd) {
		return new Copyright(holder, yearStart, yearEnd);
	}

	inline String holder() const { return holder_; }
	inline int yearStart() const { return yearStart_; }
	inline int yearEnd() const { return yearEnd_; }

private:
	Copyright(String holder, int yearStart, int yearEnd)
		: holder_(holder),
		  yearStart_(yearStart),
		  yearEnd_(yearEnd)
	{}

	String holder_;
	int yearStart_;
	int yearEnd_;
};

typedef List< Ref<Copyright> > CopyrightList;

} // namespace fclaim

#endif // FCLAIM_COPYRIGHT_H
