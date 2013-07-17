#include <fkit/stdio.h>
#include <fkit/System.h>
#include <fkit/Date.h>
#include <fkit/check.h>

using namespace fkit;

int main()
{
	{
		fout() << "Basic date formatting..." << nl;

		double t = System::now();
		String s1 = str(Date::create(Date::create(t)->time()));
		String s2 = str(Date::create(t));

		fout("%%\n") << s1;
		fout("%%\n") << s2;
		fout("%%\n") << Date::create(t)->localTime();

		check(s1 == s2);
	}

	{
		fout() << "Begin of Epoch..." << nl;

		Ref<Date> date = Date::create(0);
		fout() << date << nl;
		fout() << "year: " << date->year() << nl;
		fout() << "month: " << date->month() << nl;
		fout() << "day: " << date->day() << nl;

		check(date->year() == 1970 && date->month() == 0 && date->day() == 1 && date->valid());
	}

	return 0;
}
