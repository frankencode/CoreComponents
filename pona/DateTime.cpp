#include "Format.hpp"
#include "DateTime.hpp"

namespace pona
{

/* Every 4th year is a leap year excluding every last year in the
 * 100 year cycle (century) but not the last year in a 400 year cycle.
 */
inline bool leapYear(int y)
{
	return (y % 4 == 0) && ((y % 100 != 0) || (y % 400 == 0));
}

/** Minor improvement of the leapYear definition proposed by Sir John Herschel.
  */
inline bool herschelLeapYear(int y)
{
	return leapYear(y) && (y % 4000 != 0);
}

/** Days in a year are arbitrary distributed but weighted to make the
  * average length of a year match the length of the solar year.
  * Interestingly the length of the year is measured in days, which is a duration
  * depending on Earth's rotational speed.
  */
inline int daysInYear(int y)
{
	return 31 * 7 + 30 * 4 + 28 + leapYear(y);
}

DateTime::DateTime(Time time)
	: time_(time)
{
	const int secPerDay = 86400;
		// The seconds per day are not affected by leap seconds by definition.
		// The POSIX time speeds up or slows down to adjust to solar reality by
		// omitting a second or repeating the same second.
	
	const int t0 = 719162;                 // linear day number (starting from 0) of 1st Jan 1970
	int tx = t0 + time_.sec() / secPerDay; // linear day number of this date
	int d400 = 146097;                     // length of the 400 year cycle
	int d100 = 36524;                      // length of the 100 year cycle
	int d4 = 1461;                         // length of the 4 year cycle
	
	int tx400 = tx    % d400; // day within the 400 year cycle (starting from 0)
	int tx100 = tx400 % d100; // day within the 100 year cycle (starting from 0)
	int tx4   = tx100 % d4;   // day within the   4 year cycle (starting from 0)
	
	int c400 = tx    / d400; // 400 year cycle number within continuum (starting from 0)
	int c100 = tx400 / d100; // 100 year cycle number within 400 year cycle (starting from 0)
	int c4   = tx100 / d4;   //   4 year cycle number within 100 year cycle (starting from 0)
	
	int century = (c400 * 400 + c100 * 100) / 100;
	year_ = (century * 100) + c4 * 4 + 1;
	int ty0 = 0; // first day in year within the 4 year cycle (starting from 0)
	{
		while (true) {
			int ty1 = ty0 + daysInYear(year_); // first day of next year (starting from 0)
			if (tx4 < ty1) break;
			ty0 = ty1;
			++year_;
		}
	}
	int tyx = tx4 - ty0; // day witin this year (starting from 0)
	month_ = 1;
	int tm0 = 0; // first day of month (within this year)
	{
		const int days[] = {
			31/*Jan*/, 28/*Feb*/, 31/*Mar*/, 30/*Apr*/, 31/*May*/, 30/*Jun*/,
			31/*Jul*/, 31/*Aug*/, 30/*Sep*/, 31/*Oct*/, 30/*Nov*/, 31/*Dec*/
		};
		for (int i = 0; i < 12; ++i) {
			int tm1 = tm0 + days[i] + (i == 1) * leapYear(year_); // first day of next month
			if (tyx < tm1) break;
			++month_;
			tm0 = tm1;
		}
	}
	day_ = tyx - tm0 + 1/*adjust to start with day 1*/;
	weekday_ = (tx + 1/*adjust to start with Sunday*/) % 7;
	
	hour_ = (time_.sec() / 3600) % 24;
	min_ = (time_.sec() / 60) % 60;
	s_ = time_.sec() % 60;
	ms_ = time_.nsec() / 1000000;
}

String DateTime::iso8601() const
{
	return Format(
		"%4.:'0'%-%2.:'0'%-%2.:'0'%T"
		"%2.:'0'%:%2.:'0'%:%2.:'0'%.%2.:'0'%Z"
	) << year_ << month_ << day_
	  << hour() << min() << s() << ms() / 10;
}

} // namespace pona
