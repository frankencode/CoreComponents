#!/bin/bash
##
 # Copyright (C) 2007-2014 Frank Mertens.
 #
 # Use of this source is governed by a BSD-style license that can be
 # found in the LICENSE file.
 #
 ##

MAX_ARGS=32

let n=2
while [ $n -le $MAX_ARGS ]; do
    let m=$n-1
    let p=$n-2
    printf "inline NODE CHOICE("
    let i=0
    while [ $i -le $p ]; do
        printf "NODE choice$i, "
        let i=i+1
    done
    printf "NODE choice$m) {\n"
    printf "\tNODE node = new ChoiceNode;\n"
    let i=0
    while [ $i -le $m ]; do
        printf "\tnode->appendChild(choice$i);\n"
        let i=i+1
    done
    printf "\treturn debug(node, \"Choice\");\n"
    printf "}\n"
    let n=n+1
done

let n=2
while [ $n -le $MAX_ARGS ]; do
    let m=$n-1
    let p=$n-2
    printf "inline NODE GLUE("
    let i=0
    while [ $i -le $p ]; do
        printf "NODE child$i, "
        let i=i+1
    done
    printf "NODE child$m) {\n"
    printf "\tNODE node = new GlueNode;\n"
    let i=0
    while [ $i -le $m ]; do
        printf "\tnode->appendChild(child$i);\n"
        let i=i+1
    done
    printf "\treturn debug(node, \"Glue\");\n"
    printf "}\n"
    let n=n+1
done
