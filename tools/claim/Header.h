#ifndef FCLAIM_HEADER_H
#define FCLAIM_HEADER_H

#include <flux/Token.h>
#include <flux/String.h>

namespace fclaim
{

using namespace flux;

class Header: public Object
{
public:
	inline static Ref<Header> create(String path, Token *token, String text, String message) {
		return new Header(path, token, text, message);
	}

	inline String path() const { return path_; }
	inline Token *token() const { return token_; }
	inline String text() const { return text_; }
	inline String message() const { return message_; }

private:
	Header(String path, Token *token, String text, String message)
		: path_(path),
		  token_(token),
		  text_(text),
		  message_(message)
	{}

	String path_;
	Ref<Token> token_;
	String text_;
	String message_;
};

} // namespace fclaim

#endif // FCLAIM_HEADER_H
