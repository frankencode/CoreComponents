/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef PONA_IOSYNTAXSUGAR_HPP
#define PONA_IOSYNTAXSUGAR_HPP

#include "Atoms.hpp"
#include "String.hpp"
#include "StdStream.hpp"
#include "LinePrinter.hpp"
#include "LineParser.hpp"

namespace pona
{

inline void print(String format, Ref<LineSink> sink = output()) { sink->write(format); }

template<class A>
inline void print(String format, A a, Ref<LineSink> sink = output()) { LinePrinter lp(format); sink->write(lp % a); }

template<class A, class B>
inline void print(String format, A a, B b, Ref<LineSink> sink = output()) { LinePrinter lp(format); sink->write(lp % a % b); }

template<class A, class B, class C>
inline void print(String format, A a, B b, C c, Ref<LineSink> sink = output()) { LinePrinter lp(format); sink->write(lp % a % b % c); }

template<class A, class B, class C, class D>
inline void print(String format, A a, B b, C c, D d, Ref<LineSink> sink = output()) { LinePrinter lp(format); sink->write(lp % a % b % c % d); }

template<class A, class B, class C, class D, class E>
inline void print(String format, A a, B b, C c, D d, E e, Ref<LineSink> sink = output()) { LinePrinter lp(format); sink->write(lp % a % b % c % d % e); }

template<class A, class B, class C, class D, class E, class F>
inline void print(String format, A a, B b, C c, D d, E e, F f, Ref<LineSink> sink = output()) { LinePrinter lp(format); sink->write(lp % a % b % c % d % e % f); }

template<class A, class B, class C, class D, class E, class F, class G>
inline void print(String format, A a, B b, C c, D d, E e, F f, G g, Ref<LineSink> sink = output()) { LinePrinter lp(format); sink->write(lp % a % b % c % d % e % f % g); }

template<class A, class B, class C, class D, class E, class F, class G, class H>
inline void print(String format, A a, B b, C c, D d, E e, F f, G g, H h, Ref<LineSink> sink = output()) { LinePrinter lp(format); sink->write(lp % a % b % c % d % e % f % g % h); }

template<class A, class B, class C, class D, class E, class F, class G, class H, class I>
inline void print(String format, A a, B b, C c, D d, E e, F f, G g, H h, I i, Ref<LineSink> sink = output()) { LinePrinter lp(format); sink->write(lp % a % b % c % d % e % f % g % h % i); }

template<class A, class B, class C, class D, class E, class F, class G, class H, class I, class J>
inline void print(String format, A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, Ref<LineSink> sink = output()) { LinePrinter lp(format); sink->write(lp % a % b % c % d % e % f % g % h % i % j); }

template<class A>
inline int scan(String format, A* a, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	lp.arg(*a);
	return !lp.syntaxError();
}

template<class A, class B>
inline int scan(String format, A* a, B* b, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(*a); n += !lp.syntaxError();
	lp.arg(*b); n += !lp.syntaxError();
	return n;
}

template<class A, class B, class C>
inline int scan(String format, A* a, B* b, C* c, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(*a); n += !lp.syntaxError();
	lp.arg(*b); n += !lp.syntaxError();
	lp.arg(*c); n += !lp.syntaxError();
	return n;
}

template<class A, class B, class C, class D>
inline int scan(String format, A* a, B* b, C* c, D* d, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(*a); n += !lp.syntaxError();
	lp.arg(*b); n += !lp.syntaxError();
	lp.arg(*c); n += !lp.syntaxError();
	lp.arg(*d); n += !lp.syntaxError();
	return n;
}

template<class A, class B, class C, class D, class E>
inline int scan(String format, A* a, B* b, C* c, D* d, E* e, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(*a); n += !lp.syntaxError();
	lp.arg(*b); n += !lp.syntaxError();
	lp.arg(*c); n += !lp.syntaxError();
	lp.arg(*d); n += !lp.syntaxError();
	lp.arg(*e); n += !lp.syntaxError();
	return n;
}

template<class A, class B, class C, class D, class E, class F>
inline int scan(String format, A* a, B* b, C* c, D* d, E* e, F* f, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(*a); n += !lp.syntaxError();
	lp.arg(*b); n += !lp.syntaxError();
	lp.arg(*c); n += !lp.syntaxError();
	lp.arg(*d); n += !lp.syntaxError();
	lp.arg(*e); n += !lp.syntaxError();
	lp.arg(*f); n += !lp.syntaxError();
	return n;
}

template<class A, class B, class C, class D, class E, class F, class G>
inline int scan(String format, A* a, B* b, C* c, D* d, E* e, F* f, G* g, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(*a); n += !lp.syntaxError();
	lp.arg(*b); n += !lp.syntaxError();
	lp.arg(*c); n += !lp.syntaxError();
	lp.arg(*d); n += !lp.syntaxError();
	lp.arg(*e); n += !lp.syntaxError();
	lp.arg(*f); n += !lp.syntaxError();
	lp.arg(*g); n += !lp.syntaxError();
	return n;
}

template<class A, class B, class C, class D, class E, class F, class G, class H>
inline int scan(String format, A* a, B* b, C* c, D* d, E* e, F* f, G* g, H* h, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(*a); n += !lp.syntaxError();
	lp.arg(*b); n += !lp.syntaxError();
	lp.arg(*c); n += !lp.syntaxError();
	lp.arg(*d); n += !lp.syntaxError();
	lp.arg(*e); n += !lp.syntaxError();
	lp.arg(*f); n += !lp.syntaxError();
	lp.arg(*g); n += !lp.syntaxError();
	lp.arg(*h); n += !lp.syntaxError();
	return n;
}

template<class A, class B, class C, class D, class E, class F, class G, class H, class I>
inline int scan(String format, A* a, B* b, C* c, D* d, E* e, F* f, G* g, H* h, I* i, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(*a); n += !lp.syntaxError();
	lp.arg(*b); n += !lp.syntaxError();
	lp.arg(*c); n += !lp.syntaxError();
	lp.arg(*d); n += !lp.syntaxError();
	lp.arg(*e); n += !lp.syntaxError();
	lp.arg(*f); n += !lp.syntaxError();
	lp.arg(*g); n += !lp.syntaxError();
	lp.arg(*h); n += !lp.syntaxError();
	lp.arg(*i); n += !lp.syntaxError();
	return n;
}

template<class A, class B, class C, class D, class E, class F, class G, class H, class I, class J>
inline int scan(String format, A* a, B* b, C* c, D* d, E* e, F* f, G* g, H* h, I* i, J* j, Ref<LineSource> source = input())
{
	LineParser lp(source->readLine());
	int n = 0;
	lp.arg(*a); n += !lp.syntaxError();
	lp.arg(*b); n += !lp.syntaxError();
	lp.arg(*c); n += !lp.syntaxError();
	lp.arg(*d); n += !lp.syntaxError();
	lp.arg(*e); n += !lp.syntaxError();
	lp.arg(*f); n += !lp.syntaxError();
	lp.arg(*g); n += !lp.syntaxError();
	lp.arg(*h); n += !lp.syntaxError();
	lp.arg(*i); n += !lp.syntaxError();
	lp.arg(*j); n += !lp.syntaxError();
	return n;
}

} // namespace pona

#endif // PONA_IOSYNTAXSUGAR_HPP
