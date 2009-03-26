#ifndef PONA_FORMATSYNTAXSUGAR_HPP
#define PONA_FORMATSYNTAXSUGAR_HPP

#include "atoms"
#include "String.hpp"
#include "Format.hpp"
#include "StandardStreams.hpp"

namespace pona
{

inline void print(String text, Ref<LineSink> sink = output()) { sink->write(text); }

template<class T1>
inline void print(String templateText, T1 x1, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1;
	sink->write(format);
}

template<class T1, class T2>
inline void print(String templateText, T1 x1, T2 x2, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2;
	sink->write(format);
}

template<class T1, class T2, class T3>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18 << x19;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18 << x19 << x20;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18 << x19 << x20 << x21;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18 << x19 << x20 << x21 << x22;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18 << x19 << x20 << x21 << x22 << x23;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18 << x19 << x20 << x21 << x22 << x23 << x24;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18 << x19 << x20 << x21 << x22 << x23 << x24 << x25;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18 << x19 << x20 << x21 << x22 << x23 << x24 << x25 << x26;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18 << x19 << x20 << x21 << x22 << x23 << x24 << x25 << x26 << x27;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18 << x19 << x20 << x21 << x22 << x23 << x24 << x25 << x26 << x27 << x28;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28, T29 x29, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18 << x19 << x20 << x21 << x22 << x23 << x24 << x25 << x26 << x27 << x28 << x29;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29, class T30>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28, T29 x29, T30 x30, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18 << x19 << x20 << x21 << x22 << x23 << x24 << x25 << x26 << x27 << x28 << x29 << x30;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29, class T30, class T31>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28, T29 x29, T30 x30, T31 x31, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18 << x19 << x20 << x21 << x22 << x23 << x24 << x25 << x26 << x27 << x28 << x29 << x30 << x31;
	sink->write(format);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29, class T30, class T31, class T32>
inline void print(String templateText, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28, T29 x29, T30 x30, T31 x31, T32 x32, Ref<LineSink> sink = output())
{
	Format format(templateText);
	format << x1 << x2 << x3 << x4 << x5 << x6 << x7 << x8 << x9 << x10 << x11 << x12 << x13 << x14 << x15 << x16 << x17 << x18 << x19 << x20 << x21 << x22 << x23 << x24 << x25 << x26 << x27 << x28 << x29 << x30 << x31 << x32;
	sink->write(format);
}

} // namespace pona

#endif // PONA_FORMATSYNTAXSUGAR_HPP

