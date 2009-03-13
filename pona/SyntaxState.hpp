#ifndef PONA_SYNTAXSTATE_HPP
#define PONA_SYNTAXSTATE_HPP

#include "atoms"
#include "Vector.hpp"

namespace pona
{

template<class Char>
class SyntaxState: public Instance
{
public:
	SyntaxState()
		: language_(-1)
	{}
	
	SyntaxState(int language, int numFlags, int numChars, int numStrings, Ref<SyntaxState> parent = 0)
		: language_(language),
		  flags_(numFlags),
		  chars_(numChars),
		  strings_(numStrings)
	{
		if (parent)
			parent->child_ = this;
	}
	
	inline int language() const { return language_; }
	
	inline bool* flag(int id) { return flags_.at(id); }
	inline Char* character(int id) { return chars_.at(id); }
	inline Vector<Char>* string(int id) { return strings_.at(id); }
	
	inline Ref<SyntaxState> child() const { return child_; }
	inline void setChild(Ref<SyntaxState> state) { child_ = state; }
	
	bool equals(const SyntaxState& b) const
	{
		bool equal = (language_ == b.language_);
		if (equal) equal = (flags_ == b.flags_);
		if (equal) equal = (chars_ == b.chars_);
		if (equal) equal = (strings_ == b.strings_);
		if (equal) {
			if (child_) {
				if (b.child_) {
					if (child_ != b.child_)
						equal = equal && child_->equals(*b.child_);
				}
				else
					equal = false;
			}
			else {
				equal = equal && (!b.child_);
			}
		}
		return equal;
	}
	
	void copy(const SyntaxState& b)
	{
		language_ = b.language_;
		flags_ = b.flags_;
		chars_ = b.chars_;
		strings_ = b.strings_;
		if (b.child_) {
			child_ = new SyntaxState;
			child_->copy(*b.child_);
		}
		else
			child_ = 0;
	}
	
private:
	int language_;
	Vector<bool> flags_;
	Vector<Char> chars_;
	Vector< Vector<Char> > strings_;
	Ref<SyntaxState, Owner> child_;
};

} // namespace pona

#endif // PONA_SYNTAXSTATE_HPP
