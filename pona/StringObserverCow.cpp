#include "String.hpp"
#include "StringObserverCow.hpp"

namespace pona
{

StringObserverCow::StringObserverCow(Ref<String> string)
	: string_(string)
{}

bool StringObserverCow::beforePush(int i, int n, const Char* v)
{
	if (string_->get()->refCount() > 1) {
		string_->set(string_->get()->copy());
		string_->get()->push(i, n, v);
		return false;
	}
	return true;
}

bool StringObserverCow::beforePop(int i, int n)
{
	if (string_->get()->refCount() > 1) {
		string_->set(string_->get()->copy());
		string_->get()->pop(i, n);
		return false;
	}
	return true;
}

bool StringObserverCow::beforeWrite(int i, int n, const Char* v)
{
	if (string_->get()->refCount() > 1) {
		string_->set(string_->get()->copy());
		string_->get()->write(i, n, v);
		return false;
	}
	return true;
}

} // namespace pona
