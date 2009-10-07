#ifndef PONA_SEQUENCE_HPP
#define PONA_SEQUENCE_HPP

#include "atoms"

namespace pona
{

template<class T>
class Sequence: public virtual Instance
{
public:
	typedef T Element;
	
	virtual ~Sequence() {}
	virtual bool def(int i) const = 0;
	virtual Element get(int i) const = 0;
	virtual void set(int i, Element e) = 0;
};

} // namespace pona

#endif // PONA_SEQUENCE_HPP
