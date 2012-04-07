#ifndef FTL_WIRE_HPP
#define FTL_WIRE_HPP

#include "Singleton.hpp"
#include "Syntax.hpp"
#include "ByteArray.hpp"

namespace ftl
{

class Wire: public Syntax<ByteArray>::Definition, public Singleton<Wire>
{
public:


private:
	friend class Singleton<Wire>;
	Wire();
	int char_;
	int string_;
	int concatenation_;
	int name_;
	int class_;
	int type_;
	int value_;
	int member_;
	int array_;
	int object_;
	int message_;
};

} // namespace ftl

#endif // FTL_WIRE_HPP
