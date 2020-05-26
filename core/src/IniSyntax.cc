#include <cc/debug> // DEBUG
#include <cc/LineSource>
#include <cc/IniSyntax>

namespace cc {

void IniSyntax::parse(const string &text)
{
    auto source = LineSource::open(text);

    for (string line; source->read(&line);) {
        mutate(line)->trimInsitu(" \t", " \t");
        if (line->count() == 0) {
            // ignore empty line
        }
        else if (line->startsWith('[')) {
            mutate(line)->trimInsitu("[ \t", "] \t");
            enterSection(line);
        }
        else if (line->startsWith(';')) {
            // ignore comment
        }
        else {
            int i = 0;
            if (line->find('=', &i)) {
                string key = line->copy(0, i);
                string value = line->copy(i + 1, line->count());
                mutate(key)->trimInsitu("", " \t");
                mutate(value)->trimInsitu(" \t", "");
                establish(key, value);
            }
            else {
                handleError(source->offset(), line);
            }
        }
    }
}

} // namespace cc
