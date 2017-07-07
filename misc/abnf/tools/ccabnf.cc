/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/File>
#include <cc/syntax/SyntaxDebugger>
#include <cc/abnf/AbnfCompiler>

using namespace cc;
using namespace cc::abnf;

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
