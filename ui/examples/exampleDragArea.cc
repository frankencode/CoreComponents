#include <cc/Application>
#include <cc/DragArea>
#include <cc/Label>

int main()
{
    using namespace cc;

    Window{
        DragArea{640, 480}
        .setup([](auto &self){
            const int n = 10;
            for (int i = 0; i < n; ++i) {
                self.add(
                    Label{"Drag me!"}
                    .margin(dp(40))
                    .paper(Color::fromHsv(360. * i / n, 0.4, 1.))
                );
            }
        })
    }.show();

    return Application{}.run();
}
