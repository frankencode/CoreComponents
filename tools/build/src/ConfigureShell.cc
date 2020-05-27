/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/format>
#include <cc/Process>
#include "ConfigureShell.h"

namespace ccbuild {

ConfigureShell *ConfigureShell::instance()
{
    return Singleton<ConfigureShell>::instance();
}

ConfigureShell::ConfigureShell():
    verbose_{false},
    cache_{Cache::create()}
{}

string ConfigureShell::run(const string &shellCommand)
{
    if (shellCommand == "") return "";

    string text;
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
        throw string{
            format{} << "Configure command failed with status = " << status << " (\"" << shellCommand << "\")"
        };
    }

    insert(shellCommand, text);

    return text;
}

bool ConfigureShell::lookup(const string &command, string *output) const
{
    return cache_->lookup(command, output);
}

void ConfigureShell::insert(const string &command, const string &output)
{
    cache_->insert(command, output);
}

} // namespace ccbuild
