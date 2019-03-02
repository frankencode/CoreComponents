#include <cc/LineSource>
#include <cc/meta/MetaObject>
#include <cc/meta/IniSyntax>

namespace cc {
namespace meta {

Variant IniSyntax::parse(const CharArray* text)
{
    auto rootObject = MetaObject::create();
    auto currentObject = rootObject;

    for (String line: LineSource::open(text)) {
        mutate(line)->trimInsitu();
        if (line->count() == 0) {
            // ignore empty line
        }
        else if (line->beginsWith('[')) {
            // read section name
            mutate(line)->trimInsitu("[", "]");
            mutate(line)->trimInsitu();
            if (!rootObject->lookup(line, &currentObject)) {
                currentObject = MetaObject::create();
                rootObject->insert(line, currentObject);
            }
        }
        else if (line->beginsWith(';')) {
            // ignore comment
        }
        else {
            int i = 0;
            if (line->search('=', &i)) {
                // read key, value pair
                String key = line->copy(0, i);
                Variant value = Variant::read(line->copy(i + 1, line->count()));
                currentObject->establish(key, value);
            }
            else {
                // ignore rubbish line
            }
        }
    }

    return rootObject;
}

}} // namespace cc::meta
