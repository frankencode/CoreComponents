#include <cc/Flickable>
#include <cc/FlickableIndicator>
#include <cc/Label>
#include <cc/layout>

int main()
{
    using namespace cc;

    return
        Flickable{
            sp(640), sp(480),
            Column{}
            .populate([](auto target) {
                for (int i = 0; i < 100; ++i) {
                    target.add(Label{"Item " + str(i + 1)});
                }
            })
        }
        .add(FlickableIndicator{})
        .run();
}
