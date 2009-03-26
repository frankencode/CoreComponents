#! /bin/bash

MAX_ARGS=32

cat <<EOD
#ifndef PONA_FORMATSYNTAXSUGAR_HPP
#define PONA_FORMATSYNTAXSUGAR_HPP

#include "atoms"
#include "String.hpp"
#include "Format.hpp"
#include "StandardStreams.hpp"

namespace pona
{

EOD

printf "inline void print(String text, Ref<LineSink> sink = output()) { sink->write(text); }\n"
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
	printf "inline void print(String templateText, "
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
	printf "	Format format(templateText);\n"
	printf "	format"
	let i=1
	while [ $i -le $n ]; do
		printf " << x$i"
		let i=i+1
	done
	printf ";\n"
	printf "	sink->write(format);\n"
	printf "}\n"
	printf "\n"
	let n=n+1
done

cat <<EOD
} // namespace pona

#endif // PONA_FORMATSYNTAXSUGAR_HPP

EOD
