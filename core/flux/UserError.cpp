/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Exception.h"
#include "Format.h"
#include "UserError.h"

namespace flux
{

SystemError::SystemError(String details)
	: UserError(Format() << systemError() << ((details != "") ? ": " : " ") << details)
{}

} // namespace flux
