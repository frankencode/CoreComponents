/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/SystemPrerequisite>
#include <cc/str>

namespace cc::build {

SystemPrerequisite::State::State(const MetaObject &object, const String &projectPath, BuildOption options):
    BuildParameters::State{object, projectPath, options},
    name_{object("name").to<String>()},
    origName_{name_},
    optional_{object("optional").to<bool>()},
    cascade_{object("cascade").to<bool>()},
    compileFlagsConfigure_{object("compile-flags-configure").to<String>()},
    linkFlagsConfigure_{object("link-flags-configure").to<String>()},
    versionConfigure_{object("version-configure").to<String>()},
    versionMin_{object("version-min").to<Version>()},
    versionMax_{object("version-max").to<Version>()},
    probe_{object("probe").to<String>()},
    configure_{object("configure").to<String>()}
{
    if (name_ == "" && libraries_.count() == 1)
        name_ = libraries_.at(0);
    //if (name_ == "" && configure_ != "")
    //    name_ = configure_.baseName();
    if (name_ == "") name_ = hex(uint64_t(this));

    autoConfigure_ =
        origName_                   != "" &&
        compileFlagsConfigure_      == "" &&
        linkFlagsConfigure_         == "" &&
        versionConfigure_           == "" &&
        customCompileFlags_.count() == 0  &&
        customLinkFlags_.count()    == 0  &&
        probe_                      == "" &&
        configure_                  == "";
}

} // namespace cc::build
