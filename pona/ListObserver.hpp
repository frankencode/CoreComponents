#ifndef PONA_LISTOBSERVER_HPP
#define PONA_LISTOBSERVER_HPP

#include "atoms"

namespace pona
{

class ListObserver: public virtual Instance
{
public:
	virtual void beforePush(int i, int n) {}
	virtual void afterPush(int i, int n) {}
	
	virtual void beforePop(int i, int n) {}
	virtual void afterPop(int i, int n) {}
	
	virtual void beforeWrite(int i, int n) {}
	virtual void afterWrite(int i, int n) {}
};

} // namespace pona

#endif // PONA_LISTOBSERVER_HPP
