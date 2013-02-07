#! /bin/bash
##
 # PrintDebug.sh -- generates PrintDebug.hpp
 #
 # Copyright (c) 2007-2012, Frank Mertens
 #
 # See ../../COPYING for the license.
 ##

MAX_ARGS=32

cat <<EOD
/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#ifndef FTL_PRINTDEBUG_HPP
#define FTL_PRINTDEBUG_HPP

#include "String.hpp"
#include "Format.hpp"
#include "StandardStreams.hpp"

namespace ftl
{

EOD

######################### print() ####################################

printf "inline void print(String text) { output()->write(text); }\n"
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
		printf "const T$i &x$i"
		if [ $i -ne $n ]; then
			printf ", "
		fi
		let i=i+1
	done
	printf ")\n"
	printf "{\n"
	printf "	Ref<Format> format = Format::create(templateText);\n"
	printf "	format"
	let i=1
	while [ $i -le $n ]; do
		printf " << x$i"
		let i=i+1
	done
	printf ";\n"
	printf "	output()->write(format->join());\n"
	printf "}\n"
	printf "\n"
	let n=n+1
done

######################### printTo() ##################################

printf "inline void printTo(LineSink *sink, String text) { sink->write(text); }\n"
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
	printf "inline void printTo(LineSink *sink, String templateText, "
	let i=1
	while [ $i -le $n ]; do
		printf "const T$i &x$i"
		if [ $i -ne $n ]; then
			printf ", "
		fi
		let i=i+1
	done
	printf ")\n"
	printf "{\n"
	printf "	Ref<Format> format = Format::create(templateText);\n"
	printf "	format"
	let i=1
	while [ $i -le $n ]; do
		printf " << x$i"
		let i=i+1
	done
	printf ";\n"
	printf "	sink->write(format->join());\n"
	printf "}\n"
	printf "\n"
	let n=n+1
done

######################### debug() ####################################

printf "inline void debug(String text) {\n"
printf "#ifndef NDEBUG\n"
printf "	output()->write(text);\n"
printf "#endif // NDEBUG\n"
printf "}\n"
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
	printf "inline void debug(String templateText, "
	let i=1
	while [ $i -le $n ]; do
		printf "const T$i &x$i"
		if [ $i -ne $n ]; then
			printf ", "
		fi
		let i=i+1
	done
	printf ")\n"
	printf "{\n"
	printf "#ifndef NDEBUG\n"
	printf "	Ref<Format> format = Format::create(templateText);\n"
	printf "	format"
	let i=1
	while [ $i -le $n ]; do
		printf " << x$i"
		let i=i+1
	done
	printf ";\n"
	printf "	output()->write(format->join());\n"
	printf "#endif // NDEBUG\n"
	printf "}\n"
	printf "\n"
	let n=n+1
done

cat <<EOD
} // namespace ftl

#endif // FTL_PRINTDEBUG_HPP

EOD
