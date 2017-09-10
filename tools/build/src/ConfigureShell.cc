/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/stdio>
#include <cc/Process>
#include <cc/SubProcess>
#include "ConfigureShell.h"

namespace ccbuild {

ConfigureShell *ConfigureShell::instance()
{
    return Singleton<ConfigureShell>::instance();
}

ConfigureShell::ConfigureShell():
    verbose_(false),
    cache_(Cache::create())
{}

String ConfigureShell::run(String shellCommand, bool *ok)
{
    if (shellCommand == "") return "";

    String text;
    if (lookup(shellCommand, &text))
        return text;

    Ref<SubProcess> sub = SubProcess::stage()
        ->setArgs(
            StringList::create()
                << Process::env("SHELL")
                << "-c"
                << shellCommand
        )
        ->open();

    text = sub->readAll()->trim();
    int status = sub->wait();
    if (status != 0) {
        if (verbose_ || ok != nullptr) {
            ferr() << "Configure command failed with status = " << status << " (\"" << shellCommand << "\")" << nl;
            if (ok != nullptr) *ok = false;
        }
        return "";
    }

    if (ok != nullptr) *ok = true;
    insert(shellCommand, text);

    return text;
}

bool ConfigureShell::lookup(String command, String *output) const
{
    return cache_->lookup(command, output);
}

void ConfigureShell::insert(String command, String output)
{
    cache_->insert(command, output);
}

} // namespace ccbuild
