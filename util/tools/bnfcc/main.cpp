/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/SyntaxDebugger>
#include <flux/abnf/AbnfCompiler>

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
