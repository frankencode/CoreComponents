#ifndef PONA_CLONEABLE_HPP
#define PONA_CLONEABLE_HPP

#include "atoms"

namespace pona
{

class PONA_API Cloneable: public virtual Instance
{
public:
	virtual Ref<Instance, Owner> clone() = 0;
};

} // namespace pona

#endif // PONA_CLONEABLE_HPP
