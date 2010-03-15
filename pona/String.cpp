#include "Utf8Source.hpp"
#include "Crc32.hpp"
#include "FormatSyntax.hpp"
#include "IntegerLiteral.hpp"
#include "FloatLiteral.hpp"
#include "Variant.hpp"
#include "String.hpp"

namespace pona
{

String::String(const Variant& b)
	: Super(b.toInstance<Media>())
{}

void String::validate(const char* data, int size)
{
	if (size < 0) size = pona::strlen(data);
	if (size > 0) {
		Utf8Source source(data, size);
		while (source.numBytesRead() < size)
			source.readChar();
	}
}

void String::assign(Ref<StringList> parts, const char* glue)
{
	int glueSize = pona::strlen(glue);
	validate(glue, glueSize);
	if (parts->length() > 0) {
		int size = 0;
		for (StringList::Index i = parts->first(); parts->def(i); ++i)
			size += parts->at(i)->size();
		size += (parts->length() - 1) * glueSize;
		set(new Media(size));
		char* p = media()->data();
		for (StringList::Index i = parts->first(); parts->def(i); ++i) {
			String part = parts->at(i);
			pona::memcpy(p, part->data(), part->size());
			p += part->size();
			if (parts->def(i + 1)) {
				pona::memcpy(p, glue, glueSize);
				p += glueSize;
			}
		}
		check(p == media()->data() + size);
	}
	else {
		set(new Media);
	}
}

String String::deepCopy() const
{
	String b;
	b.Super::set(new Media(media()->data(), media()->size()));
	return b;
}

String::Index String::find(const Index& index, const char* pattern) const
{
	if (!index.valid() || media()->empty()) return Index();
	check(index.data() == media()->data());
	const char* t = index.pos(); // text pos
	const char* m = pattern; // match pos
	while ((*t) && (*m)) {
		if (*t == *m)
			++m;
		else
			m = pattern;
		++t;
	}
	return (*m) ? Index() : Index(media()->data(), t - (m - pattern));
}

Ref<StringList, Owner> String::split(const char* pattern) const
{
	Ref<StringList, Owner> parts = new StringList;
	Index index0 = first();
	int patternSize = pona::strlen(pattern);
	while (index0.valid()) {
		Index index1 = find(index0, pattern);
		if (!index1.valid()) break;
		parts->append(String(index0, index1));
		index0 = Index(media()->data(), index1.pos() + patternSize);
	}
	if (index0.valid())
		parts->append(String(index0, end()));
	else
		parts->append(String());
	return parts;
}

int String::toInt(bool* ok) const
{
	bool h;
	if (!ok) ok = &h;
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (formatSyntax()->integerLiteral()->match(*this, 0, &i1, &value, &sign)) {
		 *ok = (value <= uint64_t(intMax)) && (i1 == media()->size());
	}
	else  {
		*ok = false;
	}
	return sign * int(value);
}

double String::toFloat(bool* ok) const
{
	return toFloat64(ok);
}

int64_t String::toInt64(bool* ok) const
{
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (formatSyntax()->integerLiteral()->match(*this, 0, &i1, &value, &sign)) {
		if (ok)
			*ok = ((value & (uint64_t(1) << 63)) != 0) && (i1 == media()->size());
	}
	else {
		if (ok)
			*ok = false;
	}
	return sign * value;
}

uint64_t String::toUInt64(bool* ok) const
{
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (formatSyntax()->integerLiteral()->match(*this, 0, &i1, &value, &sign)) {
		if (ok)
			*ok = (sign == 1) && (i1 == media()->size());
	}
	else {
		if (ok)
			*ok = false;
	}
	return value;
}

float64_t String::toFloat64(bool* ok) const
{
	float64_t value = 0.;
	int i1 = 0;
	if (formatSyntax()->floatingPointLiteral()->match(*this, 0, &i1, &value)) {
		if (ok)
			*ok = (i1 == media()->size());
	}
	else {
		if (ok)
			*ok = false;
	}
	return value;
}

String String::toLower() const
{
	String s2(media()->size());
	for (int i = 0, n = media()->size(); i < n; ++i)
		s2->set(i, pona::toLower((*this)->at(i)));
	return s2;
}

String String::toUpper() const
{
	String s2(media()->size());
	for (int i = 0, n = media()->size(); i < n; ++i)
		s2->set(i, pona::toUpper((*this)->at(i)));
	return s2;
}

String String::stripLeadingSpace() const
{
	int n = media()->size();
	while (n > 0) {
		if (!pona::isSpace((*this)->at(-n))) break;
		--n;
	}
	return String(*this, media()->size() - n, n);
}

String String::stripTrailingSpace() const
{
	int n = media()->size();
	while (n > 0) {
		if (!pona::isSpace((*this)->at(n - 1))) break;
		--n;
	}
	return String(*this, 0, n);
}

} // namespace pona
