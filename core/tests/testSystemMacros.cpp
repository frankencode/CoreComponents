#include <ftl/PrintDebug.hpp>

namespace ftl
{

int main()
{
	// http://predef.sourceforge.net/preos.html
	#ifdef __linux
	print("\"__linux\": def\n");
	#else
	print("\"__linux\": undef\n");
	#endif
	#ifdef __sun
	print("\"__sun\": def\n");
	#else
	print("\"__sun\": undef\n");
	#endif
	#ifdef __bsdi__
	print("\"__bsdi__\": def\n");
	#else
	print("\"__bsdi__\": undef\n");
	#endif
	#ifdef __MACH__
	print("\"__MACH__\": def\n");
	#else
	print("\"__MACH__\": undef\n");
	#endif
	#ifdef __APPLE__
	print("\"__APPLE_\": def\n");
	#else
	print("\"__APPLE_\": undef\n");
	#endif
	#ifdef __FreeBSD__
	print("\"__FreeBSD__\": def\n");
	#else
	print("\"__FreeBSD__\": undef\n");
	#endif
	#ifdef __OpenBSD__
	print("\"__OpenBSD__\": def\n");
	#else
	print("\"__OpenBSD__\": undef\n");
	#endif
	return 0;
}

} // namespace ftl

int main()
{
	return ftl::main();
}
