/*
 * CommandOption.hpp -- command line argument
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../../LICENSE for the license.
 */
#ifndef PONA_COMMANDOPTION_HPP
#define PONA_COMMANDOPTION_HPP

#include "atoms"
#include "String.hpp"
#include "Variant.hpp"

namespace pona
{

class CommandLine;

class CommandOption: public Instance
{
public:
	inline char shortName() const { return shortName_; }
	inline String longName() const { return longName_; }
	inline String description() const { return description_; }
	
	inline Variant defaultValue() const { return defaultValue_; }
	inline void setDefaultValue(const Variant& value) { defaultValue_ = value; }
	
	inline Variant value() const { return value_; }
	inline void setValue(const Variant& value) { value_ = value; }
	
private:
	friend class CommandLine;
	char shortName_;
	String longName_;
	String description_;
	int typeMask_;
	Variant defaultValue_;
	Variant value_;
};

} // namespace pona

#endif // PONA_COMMANDOPTION_HPP
