#include <flux/stdio.h>
#include <flux/toki/Theme.h>

using namespace flux;
using namespace flux::toki;

int main()
{
	Ref<Theme> theme = Theme::load("ClassicWhite");
	return 0;
}
