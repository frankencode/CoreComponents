#ifndef FTL_XAUTHFILE_HPP
#define FTL_XAUTHFILE_HPP

#include <arpa/inet.h>
#include <ftl/String.hpp>
#include <ftl/List.hpp>

namespace ftl
{

class XAuthRecord: public Instance
{
public:
	int family;
	String host;
	int display;
	String protocol;
	String data;
};

typedef List< Ref<XAuthRecord, Owner> > XAuthRecords;

class XAuthFile: public Instance
{
public:
	XAuthFile(String path = "");
	inline String path() const { return path_; }
	inline Ref<XAuthRecords> records() const { return records_; }
	
private:
	String path_;
	Ref<XAuthRecords, Owner> records_;
};

} // namespace ftl

#endif // FTL_XAUTHFILE_HPP
