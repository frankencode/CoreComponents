#ifndef PONA_PRINTSCANFORMAT_HPP
#define PONA_PRINTSCANFORMAT_HPP

#include "atoms"
#include "String.hpp"
#include "StandardStreams.hpp"
#include "LinePrinter.hpp"
#include "LineParser.hpp"

namespace pona
{

inline void print(String format, Ref<LineSink> sink = output()) { sink->write(format); }

template<class T1>
inline void print(String format, T1 x1, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1);
	sink->write(lp);
}

template<class T1, class T2>
inline void print(String format, T1 x1, T2 x2, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2);
	sink->write(lp);
}

template<class T1, class T2, class T3>
inline void print(String format, T1 x1, T2 x2, T3 x3, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25).arg(x26);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25).arg(x26).arg(x27);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25).arg(x26).arg(x27).arg(x28);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28, T29 x29, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25).arg(x26).arg(x27).arg(x28).arg(x29);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29, class T30>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28, T29 x29, T30 x30, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25).arg(x26).arg(x27).arg(x28).arg(x29).arg(x30);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29, class T30, class T31>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28, T29 x29, T30 x30, T31 x31, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25).arg(x26).arg(x27).arg(x28).arg(x29).arg(x30).arg(x31);
	sink->write(lp);
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29, class T30, class T31, class T32>
inline void print(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28, T29 x29, T30 x30, T31 x31, T32 x32, Ref<LineSink> sink = output())
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25).arg(x26).arg(x27).arg(x28).arg(x29).arg(x30).arg(x31).arg(x32);
	sink->write(lp);
}

template<class T1>
inline int scan(String format, T1* x1, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2>
inline int scan(String format, T1* x1, T2* x2, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3>
inline int scan(String format, T1* x1, T2* x2, T3* x3, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, T19* x19, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	lp.arg(x19); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, T19* x19, T20* x20, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	lp.arg(x19); n += !lp.syntaxError();
	lp.arg(x20); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, T19* x19, T20* x20, T21* x21, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	lp.arg(x19); n += !lp.syntaxError();
	lp.arg(x20); n += !lp.syntaxError();
	lp.arg(x21); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, T19* x19, T20* x20, T21* x21, T22* x22, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	lp.arg(x19); n += !lp.syntaxError();
	lp.arg(x20); n += !lp.syntaxError();
	lp.arg(x21); n += !lp.syntaxError();
	lp.arg(x22); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, T19* x19, T20* x20, T21* x21, T22* x22, T23* x23, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	lp.arg(x19); n += !lp.syntaxError();
	lp.arg(x20); n += !lp.syntaxError();
	lp.arg(x21); n += !lp.syntaxError();
	lp.arg(x22); n += !lp.syntaxError();
	lp.arg(x23); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, T19* x19, T20* x20, T21* x21, T22* x22, T23* x23, T24* x24, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	lp.arg(x19); n += !lp.syntaxError();
	lp.arg(x20); n += !lp.syntaxError();
	lp.arg(x21); n += !lp.syntaxError();
	lp.arg(x22); n += !lp.syntaxError();
	lp.arg(x23); n += !lp.syntaxError();
	lp.arg(x24); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, T19* x19, T20* x20, T21* x21, T22* x22, T23* x23, T24* x24, T25* x25, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	lp.arg(x19); n += !lp.syntaxError();
	lp.arg(x20); n += !lp.syntaxError();
	lp.arg(x21); n += !lp.syntaxError();
	lp.arg(x22); n += !lp.syntaxError();
	lp.arg(x23); n += !lp.syntaxError();
	lp.arg(x24); n += !lp.syntaxError();
	lp.arg(x25); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, T19* x19, T20* x20, T21* x21, T22* x22, T23* x23, T24* x24, T25* x25, T26* x26, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	lp.arg(x19); n += !lp.syntaxError();
	lp.arg(x20); n += !lp.syntaxError();
	lp.arg(x21); n += !lp.syntaxError();
	lp.arg(x22); n += !lp.syntaxError();
	lp.arg(x23); n += !lp.syntaxError();
	lp.arg(x24); n += !lp.syntaxError();
	lp.arg(x25); n += !lp.syntaxError();
	lp.arg(x26); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, T19* x19, T20* x20, T21* x21, T22* x22, T23* x23, T24* x24, T25* x25, T26* x26, T27* x27, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	lp.arg(x19); n += !lp.syntaxError();
	lp.arg(x20); n += !lp.syntaxError();
	lp.arg(x21); n += !lp.syntaxError();
	lp.arg(x22); n += !lp.syntaxError();
	lp.arg(x23); n += !lp.syntaxError();
	lp.arg(x24); n += !lp.syntaxError();
	lp.arg(x25); n += !lp.syntaxError();
	lp.arg(x26); n += !lp.syntaxError();
	lp.arg(x27); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, T19* x19, T20* x20, T21* x21, T22* x22, T23* x23, T24* x24, T25* x25, T26* x26, T27* x27, T28* x28, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	lp.arg(x19); n += !lp.syntaxError();
	lp.arg(x20); n += !lp.syntaxError();
	lp.arg(x21); n += !lp.syntaxError();
	lp.arg(x22); n += !lp.syntaxError();
	lp.arg(x23); n += !lp.syntaxError();
	lp.arg(x24); n += !lp.syntaxError();
	lp.arg(x25); n += !lp.syntaxError();
	lp.arg(x26); n += !lp.syntaxError();
	lp.arg(x27); n += !lp.syntaxError();
	lp.arg(x28); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, T19* x19, T20* x20, T21* x21, T22* x22, T23* x23, T24* x24, T25* x25, T26* x26, T27* x27, T28* x28, T29* x29, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	lp.arg(x19); n += !lp.syntaxError();
	lp.arg(x20); n += !lp.syntaxError();
	lp.arg(x21); n += !lp.syntaxError();
	lp.arg(x22); n += !lp.syntaxError();
	lp.arg(x23); n += !lp.syntaxError();
	lp.arg(x24); n += !lp.syntaxError();
	lp.arg(x25); n += !lp.syntaxError();
	lp.arg(x26); n += !lp.syntaxError();
	lp.arg(x27); n += !lp.syntaxError();
	lp.arg(x28); n += !lp.syntaxError();
	lp.arg(x29); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29, class T30>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, T19* x19, T20* x20, T21* x21, T22* x22, T23* x23, T24* x24, T25* x25, T26* x26, T27* x27, T28* x28, T29* x29, T30* x30, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	lp.arg(x19); n += !lp.syntaxError();
	lp.arg(x20); n += !lp.syntaxError();
	lp.arg(x21); n += !lp.syntaxError();
	lp.arg(x22); n += !lp.syntaxError();
	lp.arg(x23); n += !lp.syntaxError();
	lp.arg(x24); n += !lp.syntaxError();
	lp.arg(x25); n += !lp.syntaxError();
	lp.arg(x26); n += !lp.syntaxError();
	lp.arg(x27); n += !lp.syntaxError();
	lp.arg(x28); n += !lp.syntaxError();
	lp.arg(x29); n += !lp.syntaxError();
	lp.arg(x30); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29, class T30, class T31>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, T19* x19, T20* x20, T21* x21, T22* x22, T23* x23, T24* x24, T25* x25, T26* x26, T27* x27, T28* x28, T29* x29, T30* x30, T31* x31, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	lp.arg(x19); n += !lp.syntaxError();
	lp.arg(x20); n += !lp.syntaxError();
	lp.arg(x21); n += !lp.syntaxError();
	lp.arg(x22); n += !lp.syntaxError();
	lp.arg(x23); n += !lp.syntaxError();
	lp.arg(x24); n += !lp.syntaxError();
	lp.arg(x25); n += !lp.syntaxError();
	lp.arg(x26); n += !lp.syntaxError();
	lp.arg(x27); n += !lp.syntaxError();
	lp.arg(x28); n += !lp.syntaxError();
	lp.arg(x29); n += !lp.syntaxError();
	lp.arg(x30); n += !lp.syntaxError();
	lp.arg(x31); n += !lp.syntaxError();
	return n;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29, class T30, class T31, class T32>
inline int scan(String format, T1* x1, T2* x2, T3* x3, T4* x4, T5* x5, T6* x6, T7* x7, T8* x8, T9* x9, T10* x10, T11* x11, T12* x12, T13* x13, T14* x14, T15* x15, T16* x16, T17* x17, T18* x18, T19* x19, T20* x20, T21* x21, T22* x22, T23* x23, T24* x24, T25* x25, T26* x26, T27* x27, T28* x28, T29* x29, T30* x30, T31* x31, T32* x32, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(x1); n += !lp.syntaxError();
	lp.arg(x2); n += !lp.syntaxError();
	lp.arg(x3); n += !lp.syntaxError();
	lp.arg(x4); n += !lp.syntaxError();
	lp.arg(x5); n += !lp.syntaxError();
	lp.arg(x6); n += !lp.syntaxError();
	lp.arg(x7); n += !lp.syntaxError();
	lp.arg(x8); n += !lp.syntaxError();
	lp.arg(x9); n += !lp.syntaxError();
	lp.arg(x10); n += !lp.syntaxError();
	lp.arg(x11); n += !lp.syntaxError();
	lp.arg(x12); n += !lp.syntaxError();
	lp.arg(x13); n += !lp.syntaxError();
	lp.arg(x14); n += !lp.syntaxError();
	lp.arg(x15); n += !lp.syntaxError();
	lp.arg(x16); n += !lp.syntaxError();
	lp.arg(x17); n += !lp.syntaxError();
	lp.arg(x18); n += !lp.syntaxError();
	lp.arg(x19); n += !lp.syntaxError();
	lp.arg(x20); n += !lp.syntaxError();
	lp.arg(x21); n += !lp.syntaxError();
	lp.arg(x22); n += !lp.syntaxError();
	lp.arg(x23); n += !lp.syntaxError();
	lp.arg(x24); n += !lp.syntaxError();
	lp.arg(x25); n += !lp.syntaxError();
	lp.arg(x26); n += !lp.syntaxError();
	lp.arg(x27); n += !lp.syntaxError();
	lp.arg(x28); n += !lp.syntaxError();
	lp.arg(x29); n += !lp.syntaxError();
	lp.arg(x30); n += !lp.syntaxError();
	lp.arg(x31); n += !lp.syntaxError();
	lp.arg(x32); n += !lp.syntaxError();
	return n;
}

template<class T1>
inline String format(String format, T1 x1)
{
	LinePrinter lp(format);
	lp.arg(x1);
	return lp;
}

template<class T1, class T2>
inline String format(String format, T1 x1, T2 x2)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2);
	return lp;
}

template<class T1, class T2, class T3>
inline String format(String format, T1 x1, T2 x2, T3 x3)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3);
	return lp;
}

template<class T1, class T2, class T3, class T4>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25).arg(x26);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25).arg(x26).arg(x27);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25).arg(x26).arg(x27).arg(x28);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28, T29 x29)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25).arg(x26).arg(x27).arg(x28).arg(x29);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29, class T30>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28, T29 x29, T30 x30)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25).arg(x26).arg(x27).arg(x28).arg(x29).arg(x30);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29, class T30, class T31>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28, T29 x29, T30 x30, T31 x31)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25).arg(x26).arg(x27).arg(x28).arg(x29).arg(x30).arg(x31);
	return lp;
}

template<class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10, class T11, class T12, class T13, class T14, class T15, class T16, class T17, class T18, class T19, class T20, class T21, class T22, class T23, class T24, class T25, class T26, class T27, class T28, class T29, class T30, class T31, class T32>
inline String format(String format, T1 x1, T2 x2, T3 x3, T4 x4, T5 x5, T6 x6, T7 x7, T8 x8, T9 x9, T10 x10, T11 x11, T12 x12, T13 x13, T14 x14, T15 x15, T16 x16, T17 x17, T18 x18, T19 x19, T20 x20, T21 x21, T22 x22, T23 x23, T24 x24, T25 x25, T26 x26, T27 x27, T28 x28, T29 x29, T30 x30, T31 x31, T32 x32)
{
	LinePrinter lp(format);
	lp.arg(x1).arg(x2).arg(x3).arg(x4).arg(x5).arg(x6).arg(x7).arg(x8).arg(x9).arg(x10).arg(x11).arg(x12).arg(x13).arg(x14).arg(x15).arg(x16).arg(x17).arg(x18).arg(x19).arg(x20).arg(x21).arg(x22).arg(x23).arg(x24).arg(x25).arg(x26).arg(x27).arg(x28).arg(x29).arg(x30).arg(x31).arg(x32);
	return lp;
}

} // namespace pona

#endif // PONA_PRINTSCANFORMAT_HPP

