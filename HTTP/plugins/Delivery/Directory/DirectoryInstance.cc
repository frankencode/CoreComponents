/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/DirectoryInstance>
#include <cc/DirectoryDelegate>
#include <cc/File>
#include <cc/FileStatus>
#include <cc/exceptions>

namespace cc {

DirectoryInstance::State::State(const MetaObject &config):
    HttpServiceInstance::State{config},
    path_{config("path").to<String>()},
    showHidden_{config("show-hidden").to<bool>()}
{
    if (path_ == "") {
        throw UsageError{"DirectoryInstance: Mandatory argument \"path\" is missing"};
    }
    path_ = path_.canonicalPath();
    if (!File::exists(path_)) {
        throw UsageError{
            Format{"DirectoryInstance: Directory path \"%%\" does not exist"} << path_
        };
    }
    if (FileStatus{path_}.type() != FileType::Directory) {
        throw UsageError{
            Format{"DirectoryInstance: Path \"%%\" does not point to a directory"} << path_
        };
    }
}

HttpServiceDelegate DirectoryInstance::State::createDelegate() const
{
    return DirectoryDelegate{};
}

DirectoryInstance::DirectoryInstance(const MetaObject &config):
    HttpServiceInstance{new State{config}}
{}

String DirectoryInstance::path() const
{
    return me().path_;;
}

bool DirectoryInstance::showHidden() const
{
    return me().showHidden_;
}

const DirectoryInstance::State &DirectoryInstance::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
