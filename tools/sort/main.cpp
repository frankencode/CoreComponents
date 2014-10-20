#include <flux/stdio>
#include <flux/Heap>

using namespace flux;

int main()
{
    Ref<StringList> list = StringList::create();
    for (String item; lineInput()->read(&item);)
        list->append(item);

    list = list->sort();

    for (int i = 0; i < list->count(); ++i)
        fout() << list->at(i) << nl;

    return 0;
}
