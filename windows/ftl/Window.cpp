#include <xcb/xcb.h>
#include <ftl/Singleton.hpp>
#include "Window.hpp"

namespace ftl
{

class XcbConnection: public Instance, public Singleton<XcbConnection>
{
public:
	inline xcb_connection_t* connection() const { return connection_; }
	
private:
	friend class Singleton<XcbConnection>;
	XcbConnection(): connection_(xcb_connect(0, 0)) {}
	~XcbConnection() { xcb_disconnect(connection_); }
	xcb_connection_t* connection_;
};

inline xcb_connection_t* xcbConnection() { return XcbConnection::instance()->connection(); }

Window::Window()
{
	
}

} // namespace ftl
