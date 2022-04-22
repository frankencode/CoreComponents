#include <cc/ui/Application>
#include <cc/ui/Flickable>
#include <cc/ui/ColumnLayout>
#include <cc/ui/Label>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    Window{
        Flickable{
            640, 480,
            Pane{}
            .setup([](auto &self) {
                for (int i = 0; i < 100; ++i) {
                    self.add(Label{"Item " + str(i + 1)});
                }
                const double gap = Font::defaultSize();
                self.layout(
                    ColumnLayout{}
                    .spacing(gap)
                    .margin(gap)
                );
            })
        }
    }.show();

    return Application{}.run();
}
