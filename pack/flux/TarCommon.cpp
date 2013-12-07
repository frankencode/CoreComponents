/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "TarCommon.h"

namespace flux
{

unsigned tarHeaderSum(ByteArray *data)
{
	unsigned sum = 0;
	for (int i = 0;       i < 148 && i < data->size(); ++i) sum += data->byteAt(i);
	for (int i = 0 + 148; i < 156 && i < data->size(); ++i) sum += ' ';
	for (int i = 0 + 156; i < 512 && i < data->size(); ++i) sum += data->byteAt(i);
	return sum;
}

} // namespace flux
