#! /bin/bash

MAX_ARGS=32

cat <<EOD
#ifndef PONA_PRINTSCANFORMAT_HPP
#define PONA_PRINTSCANFORMAT_HPP

#include "atoms"
#include "String.hpp"
#include "StandardStreams.hpp"
#include "LinePrinter.hpp"
#include "LineParser.hpp"

namespace pona
{

EOD

printf "inline void print(String format, Ref<LineSink> sink = output()) { sink->write(format); }\n"
printf "\n"
let n=1
while [ $n -le $MAX_ARGS ]; do
	printf "template<"
	let i=1
	while [ $i -le $n ]; do
		printf "class T$i"
		if [ $i -ne $n ]; then
			printf ", "
		fi
		let i=i+1
	done
	printf ">\n"
	printf "inline void print(String format, "
	let i=1
	while [ $i -le $n ]; do
		printf "T$i x$i"
		if [ $i -ne $n ]; then
			printf ", "
		fi
		let i=i+1
	done
	printf ", Ref<LineSink> sink = output())\n"
	printf "{\n"
	printf "	LinePrinter lp(format);\n"
	printf "	lp"
	let i=1
	while [ $i -le $n ]; do
		printf ".arg(x$i)"
		let i=i+1
	done
	printf ";\n"
	printf "	sink->write(lp);\n"
	printf "}\n"
	printf "\n"
	let n=n+1
done

let n=1
while [ $n -le $MAX_ARGS ]; do
	printf "template<"
	let i=1
	while [ $i -le $n ]; do
		printf "class T$i"
		if [ $i -ne $n ]; then
			printf ", "
		fi
		let i=i+1
	done
	printf ">\n"
	printf "inline int scan(String format, "
	let i=1
	while [ $i -le $n ]; do
		printf "T$i* x$i"
		if [ $i -ne $n ]; then
			printf ", "
		fi
		let i=i+1
	done
	printf ", Ref<LineSource> source = input())\n"
	printf "{\n"
	printf "	LineParser lp(source->readLine());\n"
	printf "	int n = 0;\n"
	let i=1
	while [ $i -le $n ]; do
		printf "	lp.arg(x$i); n += !lp.syntaxError();\n"
		let i=i+1
	done
	printf "	return n;\n"
	printf "}\n"
	printf "\n"
	let n=n+1
done

let n=1
while [ $n -le $MAX_ARGS ]; do
	printf "template<"
	let i=1
	while [ $i -le $n ]; do
		printf "class T$i"
		if [ $i -ne $n ]; then
			printf ", "
		fi
		let i=i+1
	done
	printf ">\n"
	printf "inline String format(String format, "
	let i=1
	while [ $i -le $n ]; do
		printf "T$i x$i"
		if [ $i -ne $n ]; then
			printf ", "
		fi
		let i=i+1
	done
	printf ")\n"
	printf "{\n"
	printf "	LinePrinter lp(format);\n"
	printf "	lp"
	let i=1
	while [ $i -le $n ]; do
		printf ".arg(x$i)"
		let i=i+1
	done
	printf ";\n"
	printf "	return lp;\n"
	printf "}\n"
	printf "\n"
	let n=n+1
done

cat <<EOD
} // namespace pona

#endif // PONA_PRINTSCANFORMAT_HPP

EOD
