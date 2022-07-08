/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/InsightDatabase>
#include <cc/File>
#include <cc/Format>
#include <cc/Variant>
#include <cc/MetaObject>
#include <cc/json>

namespace cc::build {

struct InsightDatabase::Entry
{
    String dirPath;
    String fileName;
    String command;
    bool inUse { false };
};

struct InsightDatabase::State final: public Object::State
{
    explicit State(const String &projectPath):
        path_{projectPath / "compile_commands.json"}
    {
        text_ = File::load(path_);
        Variant value;
        try { value = jsonParse(text_); }
        catch(...) {}
        if (value.is<List<Variant>>()) {
            List<Variant> list = value.to<List<Variant>>();
            for (const Variant &item: list) {
                Entry entry;
                String outputPath;
                if (!item.is<MetaObject>()) continue;
                {
                    MetaObject object = item.to<MetaObject>();
                    {
                        Variant value = object("command");
                        if (!value.is<String>()) continue;
                        entry.command = value.to<String>();
                    }
                    {
                        Variant value = object("directory");
                        if (!value.is<String>()) continue;
                        entry.dirPath = value.to<String>();
                    }
                    {
                        Variant value = object("file");
                        if (!value.is<String>()) continue;
                        entry.fileName = value.to<String>();
                    }
                    {
                        Variant value = object("output");
                        if (!value.is<String>()) continue;
                        outputPath = value.to<String>();
                    }
                }
                entries_(outputPath) = entry;
            }
        }
    }

    void insert(const String &sourcePath, const String &command, const String &outputPath)
    {
        entries_(outputPath) = Entry{sourcePath.cdUp(), sourcePath.fileName(), command, true};
        dirty_ = true;
    }

    void touch(const String &outputPath)
    {
        Locator pos;
        if (entries_.find(outputPath, &pos)) {
            entries_.at(pos).value().inUse = true;
        }
    }

    void sync()
    {
        if (!dirty_) return;
        dirty_ = false;

        Format f;
        f << "[\n";
        bool firstTime = true;
        for (const auto &pair: entries_) {
            const String &outputPath = pair.key();
            const Entry &entry = pair.value();
            if (!entry.inUse || outputPath == "") continue;
            if (firstTime) firstTime = false;
            else f << ",\n";
            f
                << "{ \"command\": \"" << entry.command << "\", "
                << "\"directory\": \"" << entry.dirPath << "\", "
                << "\"file\": \"" << entry.fileName << "\", "
                << "\"output\": \"" << outputPath << "\" }";
        }
        f << "\n]\n";

        String newText= f.join<String>();
        if (newText != text_) {
            File::save(path_, newText);
        }
    }

    String path_;
    String text_;
    Map<String, Entry> entries_;
    bool dirty_ { false };
};

InsightDatabase::InsightDatabase(const String &projectPath):
    Object{new State{projectPath}}
{}

void InsightDatabase::insert(const String &sourcePath, const String &command, const String &outputPath)
{
    me().insert(sourcePath, command, outputPath);
}

void InsightDatabase::touch(const String &outputPath)
{
    me().touch(outputPath);
}

void InsightDatabase::sync()
{
    me().sync();
}

String InsightDatabase::path() const
{
    return me().path_;
}

const InsightDatabase::State &InsightDatabase::me() const
{
    return Object::me.as<State>();
}

InsightDatabase::State &InsightDatabase::me()
{
    return Object::me.as<State>();
}

} // namespace cc::build
