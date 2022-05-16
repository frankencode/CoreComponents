#include <cc/DragArea>
#include <cc/Label>

int main()
{
    using namespace cc;

    return
        DragArea{sp(640), sp(480)}
        .populate([](auto target){
            const int n = 10;
            for (int i = 0; i < n; ++i) {
                target.add(
                    Label{"Drag me!"}
                    .margin(sp(40))
                    .paper(Color::fromHsv(360. * i / n, 0.4, 1.))
                );
            }
        })
        .run();
}
