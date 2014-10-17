/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/check>
#include <flux/SyntaxDebugger>
#include <flux/SyntaxDefinition>
#include <flux/System>

using namespace flux;

class Expression: public SyntaxDefinition
{
public:
    Expression(SyntaxDebugger *debugger = 0)
        : SyntaxDefinition(debugger)
    {
        number_ =
            DEFINE("number",
                GLUE(
                    REPEAT(0, 1, CHAR('-')),
                    REPEAT(1, 20, RANGE('0', '9'))
                )
            );

        factor_ =
            DEFINE("factor",
                CHOICE(
                    REF("number"),
                    GLUE(
                        CHAR('('),
                        REF("sum"),
                        CHAR(')')
                    )
                )
            );

        mulOp_ = DEFINE("mulOp", RANGE("*/"));

        addOp_ = DEFINE("addOp", RANGE("+-"));

        product_ =
            DEFINE("product",
                GLUE(
                    REF("factor"),
                    REPEAT(
                        GLUE(
                            REF("mulOp"),
                            REF("factor")
                        )
                    )
                )
            );

        sum_ =
            DEFINE("sum",
                GLUE(
                    REF("product"),
                    REPEAT(
                        GLUE(
                            REF("addOp"),
                            REF("product")
                        )
                    )
                )
            );

        ENTRY("sum");
        LINK();
    }

    double eval(String text)
    {
        Ref<Token> rootToken = match(text)->rootToken();
        double value = nan;

        if (rootToken) {
            text_ = text;
            value = eval(rootToken);
        }

        return value;
    }

private:
    double eval(Token *token)
    {
        double value = nan;

        if (token->rule() == sum_)
        {
            value = 0.;
            char op = '+';
            int i = 0;
            token = token->firstChild();

            while (token)
            {
                if (i % 2 == 0)
                {
                    if (op == '+')
                        value += eval(token);
                    else if (op == '-')
                        value -= eval(token);
                }
                else
                    op = text_->at(token->i0());

                token = token->nextSibling();
                ++i;
            }
        }
        else if (token->rule() == product_)
        {
            value = 1.;
            char op = '*';
            int i = 0;
            token = token->firstChild();

            while (token)
            {
                if (i % 2 == 0)
                {
                    if (op == '*')
                        value *= eval(token);
                    else if (op == '/')
                        value /= eval(token);
                }
                else
                    op = text_->at(token->i0());

                token = token->nextSibling();
                ++i;
            }
        }
        else if (token->rule() == factor_)
        {
            value = eval(token->firstChild());
        }
        else if (token->rule() == number_)
        {
            int sign = (text_->at(token->i0()) == '-') ? -1 : 1;
            value = 0;
            for (int i = token->i0() + (sign == -1); i < token->i1(); ++i) {
                value *= 10;
                value += text_->at(i) - '0';
            }
            value *= sign;
        }

        return value;
    }

    int number_;
    int factor_;
    int mulOp_;
    int addOp_;
    int product_;
    int sum_;

    String text_;
};

int main()
{
    Ref<SyntaxDebugger> debugger =
    #ifdef NDEBUG
        0;
    #else
        SyntaxDebugger::create();
    #endif
    Ref<Expression> expression = new Expression(debugger);

    double dt = System::now();

    double result = expression->eval("(-12+34)*(56-78)");

    dt = System::now() - dt;
    fout("took %% us\n") << int(dt * 1e6);
    fout("evaluates to %%\n") << result;

    check(result == -484);

    if (debugger)
        debugger->printDefinition();

    return 0;
}
