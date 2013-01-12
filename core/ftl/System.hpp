#ifndef FTL_SYSTEM_HPP
#define FTL_SYSTEM_HPP

namespace ftl
{

class System
{
public:
	static int concurrency();
	static int pageSize();
};

} // namespace ftl

#endif // FTL_SYSTEM_HPP
