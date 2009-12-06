#ifndef PONA_CLONEABLE_HPP
#define PONA_CLONEABLE_HPP

#include "Ref.hpp"

namespace pona
{

class Cloneable
{
public:
	virtual Ref<Instance, Owner> clone() = 0;
};

} // namespace pona

#endif // PONA_CLONEABLE_HPP
