/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/build/ConfigureShell>
#include <cc/Process>
#include <cc/Command>
#include <cc/Format>

namespace cc::build {

struct ConfigureShell::State: public Object::State
{
    String run(const String &command)
    {
        if (command == "") return "";

        String output;
        if (cache_.lookup(command, &output))
            return output;

        Process process {
            Command{}
            .args({Process::env("SHELL"), "-c", command})
        };

        output = process.output().readAll();
        output.trim();

        int status = process.wait();
        if (status != 0) {
            throw String{
                Format{} << "Configure command failed with status = " << status << " (\"" << command << "\")"
            };
        }

        cache_.insert(command, output);

        return output;
    }

    bool verbose_ { false };
    Map<String> cache_;
};

ConfigureShell::ConfigureShell()
{
    initOnce<State>();
}

void ConfigureShell::setVerbose(bool on)
{
    me().verbose_ = on;
}

String ConfigureShell::run(const String &command)
{
    return me().run(command);
}

ConfigureShell::State &ConfigureShell::me()
{
    return Object::me.as<State>();
}

} // namespace cc::build
