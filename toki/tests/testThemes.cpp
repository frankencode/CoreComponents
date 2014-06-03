#include <flux/stdio.h>
#include <toki/Theme.h>

using namespace fluxtoki;

int main()
{
	Ref<Theme> theme = Theme::load("ClassicWhite");
	return 0;
}
