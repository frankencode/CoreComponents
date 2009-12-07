/*
 * NonCopyable.hpp -- disallow copy-by-value logic
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_NONCOPYABLE_HPP
#define PONA_NONCOPYABLE_HPP

namespace pona
{

class NonCopyable
{
public:
	NonCopyable() {}
	
private:
	NonCopyable(const NonCopyable& b);
	const NonCopyable& operator=(const NonCopyable& b);
};

} // namespace pona

#endif // PONA_NONCOPYABLE_HPP
