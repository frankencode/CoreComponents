#include <flux/stdio>
#include <flux/LineSource>
#include <flux/Heap>

using namespace flux;

int main()
{
    Ref<StringList> list = StringList::create();
    Ref<LineSource> source = LineSource::open(stdIn());
    for (String item; source->read(&item);)
        list->append(item);

    list = list->sort();

    for (int i = 0; i < list->count(); ++i)
        fout() << list->at(i) << nl;

    return 0;
}
