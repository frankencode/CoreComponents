#ifndef FLUXDOC_GENERATOR_H
#define FLUXDOC_GENERATOR_H

#include <flux/String.h>

namespace fluxdoc
{

using namespace flux;

class Generator: public Object
{
public:
	inline String name() const { return name_; }

	virtual void run() = 0;

protected:
	Generator(String name);

	String name_;
};

} // namespace fluxdoc

#endif // FLUXDOC_GENERATOR_H
