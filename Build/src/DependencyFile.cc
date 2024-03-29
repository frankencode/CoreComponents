/*
 * Copyright (C) 2024 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/build/DependencyFile>
#include <cc/File>
#include <cc/LineSource>
#include <cc/debugging>

namespace cc::build {

struct DependencyFile::State final: public Object::State
{
    State(const String &text):
        text_{text}
    {
        text_.replace("\\\n", "");
        LineSource lineSource{text_};
        {
            String line;
            if (!lineSource.read(&line)) {
                isValid_ = false;
                return;
            }

            long i = 0;
            if (!line.find(':', &i)) {
                isValid_ = false;
                return;
            }

            line = line.copy(i + 1, line.count());
            line.simplify();
            sources_ = line.split(' ');
        }
        {
            String line;
            long i = 0;
            if (lineSource.read(&line) && line.find(':', &i)) {
                line = line.copy(i + 1, line.count());
                line.simplify();
                imports_ = line.split(' ');
            }
        }

        for (String line; lineSource.read(&line);) {
            if (line.startsWith(".PHONY")) {
                long i = 0;
                if (line.find(':', &i)) {
                    line = line.copy(i + 1, line.count());
                    line.simplify();
                    moduleName_ = line;
                }
            }
            /*else if (line.startsWith("CXX_IMPORTS")) {
                long i = 11;
                if (line.find("+=", &i)) {
                    String valuePart = line.copy(i + 2, line.count());
                    valuePart.simplify();
                    imports_ = valuePart.split(' ');
                }
            }*/
        }
    }

    String text_;
    bool isValid_ { true };
    List<String> sources_;
    List<String> imports_;
    String moduleName_;
};

DependencyFile loadFrom(const String &path)
{
    return DependencyFile{File{path}.map()};
}

DependencyFile::DependencyFile(const String &text):
    Object{new State{text}}
{}

bool DependencyFile::isValid() const
{
    return me().isValid_;
}

String DependencyFile::text() const
{
    return me().text_;
}

List<String> DependencyFile::sources() const
{
    return me().sources_;
}

List<String> DependencyFile::imports() const
{
    return me().imports_;
}

String DependencyFile::moduleName() const
{
    return me().moduleName_;
}

bool DependencyFile::isModule() const
{
    return moduleName().count() > 0;
}

DependencyFile::State &DependencyFile::me()
{
    return Object::me.as<State>();
}

const DependencyFile::State &DependencyFile::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::build
