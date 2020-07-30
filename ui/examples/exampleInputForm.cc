#include <cc/ui/Application>
#include <cc/ui/ScrollView>
#include <cc/ui/ColumnLayout>
#include <cc/ui/TextField>
#include <cc/ui/PasswordField>

int main()
{
    using namespace cc;
    using namespace cc::ui;

    ScrollView scroll{640, 480};

    ColumnLayout column{scroll};
    column->margin = dp(12);
    column->spacing = dp(12);

    scroll << TextField{"First name"};
    scroll << TextField{"Middle name"};
    scroll << TextField{"Last name"};
    scroll << TextField{"Age"};
    scroll << PasswordField{"Enter password"};
    scroll << PasswordField{"Repeat password"};
    scroll << TextField{"Street address"};
    scroll << TextField{"Zip code"};
    scroll << TextField{"Country"};

    Window::open(scroll, "Hello, world!", WindowMode::Accelerated|WindowMode::VSync);

    return Application{}->run();
}
