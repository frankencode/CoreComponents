#include <ftl/streams>
#include <ftl/utils>

namespace ftl
{

int main()
{
	/*{
		int ny = 0;
		for (int y = 1; y <= 400; ++y) {
			int nd = daysInYear(y);
			ny += nd;
			// print("y, nd, ny = %%, %%, %%\n", y, nd, ny);
		}
		print("Length of a 400 year cycle: %% days\n", ny);
		print("Average length of a solar year: %% days\n", ny / 400.);
	}
	{
		int ny = 0;
		for (int y = 1; y <= 100; ++y) {
			int nd = daysInYear(y);
			ny += nd;
			// print("y, nd, ny = %%, %%, %%\n", y, nd, ny);
		}
		print("Length of a 100 year cycle: %% days\n", ny);
	}
	{
		int ny = 0;
		for (int y = 1; y <= 4; ++y) {
			int nd = daysInYear(y);
			ny += nd;
			// print("y, nd, ny = %%, %%, %%\n", y, nd, ny);
		}
		print("Length of a 4 year cycle: %% days\n", ny);
	}
	{
		int ny = 0;
		for (int y = 1; y < 1970; ++y) ny += daysInYear(y);
		print("Linear day number (starting with day 0) of 1st Jan 1970: %%\n", ny);
		print("That must have been weekday (Mon..Sun = 0..6): %%\n", ny % 7);
	}
	{
		int ny = 0;
		int ye = 2012;
		for (int y = 1; y < ye; ++y) ny += daysInYear(y);
		print("Year %% started with weekday (Mon..Sun = 0..6): %%\n", ye, ny % 7);
	}*/
	
	print("%%\n", Date(Date(Time::now()).time()).iso8601());
	print("%%\n", Date(Time::now()).iso8601());
	print("%%\n", Date(Time::now()).localTime()->iso8601());
	
	return 0;
}

} // namespace ftl


int main()
{
	return ftl::main();
}
