#include <cc/TestSuite>
#include <cc/testing>
#include <cc/stdio>
#include <cc/System>
#include <cc/SyntaxDefinition>
#include <limits>

using namespace cc;

class Expression final: public SyntaxDefinition
{
public:
    Expression():
        SyntaxDefinition{new State}
    {}

    double eval(const String &text) const
    {
        Token root = me().match(text);
        return root ? me().eval(text, root) : std::numeric_limits<double>::quiet_NaN();
    }

private:
    struct State: public SyntaxDefinition::State
    {
        SyntaxRule number {
            Sequence{
                Repeat{0, 1, '-'},
                Repeat{1, 20, Within{'0', '9'}}
            }
        };

        SyntaxRule factor {
            Choice{
                &number,
                Sequence{'(', &sum, ')'}
            }
        };

        SyntaxRule mulOp { OneOf{'*', '/'} };
        SyntaxRule addOp { OneOf{'+', '-'} };

        SyntaxRule product {
            Sequence{
                &factor,
                Repeat{
                    Sequence{
                        &mulOp,
                        &factor
                    }
                }
            }
        };

        SyntaxRule sum {
            Sequence{
                &product,
                Repeat{
                    Sequence{
                        &addOp,
                        &product
                    }
                }
            }
        };

        State():
            SyntaxDefinition::State{&sum}
        {}

        double eval(const String &text, const Token &token) const
        {
            double value = std::numeric_limits<double>::quiet_NaN();

            if (token.rule() == sum)
            {
                value = 0.;
                char op = '+';
                for (const Token &child: token.children()) {
                    if (child.rule() == product) {
                        double nextValue = eval(text, child);
                        if (op == '+')
                            value += nextValue;
                        else if (op == '-')
                            value -= nextValue;
                    }
                    else
                        op = text.at(child.range()[0]);
                }
            }
            else if (token.rule() == product)
            {
                value = 1.;
                char op = '*';
                for (const Token &child: token.children()) {
                    if (child.rule() == factor) {
                        double nextValue = eval(text, child);
                        if (op == '*')
                            value *= nextValue;
                        else if (op == '/')
                            value /= nextValue;
                    }
                    else
                        op = text.at(child.range()[0]);
                }
            }
            else if (token.rule() == factor)
            {
                value = eval(text, token.children().first());
            }
            else if (token.rule() == number)
            {
                int sign = text.at(token.range()[0]) == '-' ? -1 : 1;
                value = 0;
                for (long i = token.range()[0] + (sign == -1); i < token.range()[1]; ++i) {
                    value *= 10;
                    value += text.at(i) - '0';
                }
                value *= sign;
            }

            return value;
        }
    };

    const State &me() const { return Object::me.as<State>(); }
};

int main(int argc, char *argv[])
{
    TestCase {
        "Calculator",
        []{
            Expression expression;
            double dt = System::now();

            double result = expression.eval("(-12+34)*(56-78)");

            dt = System::now() - dt;
            fout() << "Took " << int(dt * 1e6) << " µs" << nl;

            CC_CHECK_EQUALS(result, -484);

            #if 0
            CC_INSPECT(Token::sizeOfState());
            CC_INSPECT(Token::sizeOfState() / 16);
            CC_INSPECT(Token::sizeOfState() % 16);
            #endif
        }
    };

    return TestSuite{argc, argv}.run();
}
