#include <pona/stdio>
#include <pona/syntax>
#include <pona/misc>

namespace pona
{

class Expression: public SyntaxDefinition<String>
{
public:
	Expression()
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
		
		DEFINE_SELF("expression",
			GLUE(
				REF("sum"),
				EOI()
			)
		);
		
		LINK();
	}
	
	double eval(String text)
	{
		Ref<Token, Owner> rootToken = 0;
		double value = nan;
		
		int i0 = 0, i1 = 0;
		if (match(&text, i0, &i1, &rootToken, 0, buf_, bufSize_))
		{
			text_ = text;
			value = eval(rootToken);
		}
		
		return value;
	}
	
private:
	double eval(Ref<Token> token)
	{
		double value = nan;
		
		if (token->rule() == this->id())
		{
			value = eval(token->firstChild());
		}
		else if (token->rule() == sum_->id())
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
					op = text_.get(token->index());
				
				token = token->nextSibling();
				++i;
			}
		}
		else if (token->rule() == product_->id())
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
					op = text_.get(token->index());
				
				token = token->nextSibling();
				++i;
			}
		}
		else if (token->rule() == factor_->id())
		{
			value = eval(token->firstChild());
		}
		else if (token->rule() == number_->id())
		{
			int sign = (text_.get(token->index()) == '-') ? -1 : 1;
			value = 0;
			for (int i = token->i0() + (sign == -1); i < token->i1(); ++i) {
				value *= 10;
				value += text_.get(i) - '0';
			}
			value *= sign;
		}
		
		return value;
	}
	
	enum { bufSize_ = sizeof(Token) * 256 };
	uint8_t buf_[bufSize_];
	
	RULE number_;
	RULE factor_;
	RULE mulOp_;
	RULE addOp_;
	RULE product_;
	RULE sum_;
	
	String text_;
};

int main()
{
	Ref<Expression, Owner> expression = new Expression;
	
	TimeStamp dt = getTime();
	
	double result = expression->eval("(-12+34)*(56-78)");
	
	dt = getTime() - dt;
	output()->write(format("took %% us\n") % dt.microSeconds());
	output()->write(format("evaluates to %%\n") % result);
	
	return 0;
}

} // namespace pona

int main()
{
	return pona::main();
}
