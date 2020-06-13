/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include "ConfigureShell.h"
#include <cc/Spawn>
#include <cc/Format>

namespace ccbuild {

ConfigureShell *ConfigureShell::instance()
{
    return Singleton<ConfigureShell>::instance();
}

ConfigureShell::ConfigureShell():
    verbose_{false}
{}

String ConfigureShell::run(const String &shellCommand)
{
    if (shellCommand == "") return "";

    String text;
    if (lookup(shellCommand, &text))
        return text;

    Spawn sub {
        Command{}
        ->setArgs({Process::getEnv("SHELL"), "-c", shellCommand})
    };

    text = sub->output()->readAll()->trim();
    int status = sub->wait();
    if (status != 0) {
        throw String{
            Format{} << "Configure command failed with status = " << status << " (\"" << shellCommand << "\")"
        };
    }

    insert(shellCommand, text);

    return text;
}

bool ConfigureShell::lookup(const String &command, String *output) const
{
    return cache_->lookup(command, output);
}

void ConfigureShell::insert(const String &command, const String &output)
{
    cache_->insert(command, output);
}

} // namespace ccbuild
