/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/stdio.h>
#include <flux/SyntaxDebugger.h>
#include <flux/abnf/AbnfCompiler.h>

using namespace flux;
using namespace flux::abnf;

int main(int argc, char **argv)
{
	for (int i = 1; i < argc; ++i) {
		String text = File::open(argv[i])->map();
		Ref<AbnfCompiler> compiler = AbnfCompiler::create();
		Ref<SyntaxDebugger> debugger = SyntaxDebugger::create();
		Ref<SyntaxDefinition> definition = compiler->compile(text, debugger);
		debugger->printDefinition(false/*omitUnusedRules*/);
	}

	return 0;
}
