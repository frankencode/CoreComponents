/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/stdio>

/** \file str.h
  * \brief Debug message output
  */

/** Generate a debug message on the standard error output stream
  *
  * Examples:
  * ~~~~~~~~~~~~~
  * CC_DEBUG; // outputs the current source code position
  * CC_DEBUG << x << ", " << y; // outputs x and y
  * ~~~~~~~~~~~~~
  */
#define CC_DEBUG \
  cc::ferr("%%\n") << String(__FILE__)->fileName() << ":" << __LINE__ << ": "

/** Generate a debug message for an \a expression
  *
  * Example:
  * ~~~~~~~~~~~~~
  * CC_INSPECT(1 + 2); // outputs "<file>:<line>: 1 + 2 = 3"
  * ~~~~~~~~~~~~~
  */
#define CC_INSPECT(expression) \
  CC_DEBUG << CC_XSTR(expression) << " = " << (expression)
