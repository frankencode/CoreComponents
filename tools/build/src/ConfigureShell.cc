/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Format>
#include <cc/Process>
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

String ConfigureShell::run(String shellCommand)
{
    if (shellCommand == "") return "";

    String text;
    if (lookup(shellCommand, &text))
        return text;

    Ref<Process> sub = Process::stage()
        ->setArgs(
            StringList::create()
                << Process::getEnv("SHELL")
                << "-c"
                << shellCommand
        )
        ->open();

    text = sub->output()->readAll()->trim();
    int status = sub->wait();
    if (status != 0) {
        throw String(
            Format{} << "Configure command failed with status = " << status << " (\"" << shellCommand << "\")"
        );
    }

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
