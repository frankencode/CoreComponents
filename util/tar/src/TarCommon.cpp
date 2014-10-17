/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/tar/TarCommon>

namespace flux {
namespace tar {

unsigned tarHeaderSum(ByteArray *data)
{
	unsigned sum = 0;
	for (int i = 0;       i < 148 && i < data->count(); ++i) sum += data->byteAt(i);
	for (int i = 0 + 148; i < 156 && i < data->count(); ++i) sum += ' ';
	for (int i = 0 + 156; i < 512 && i < data->count(); ++i) sum += data->byteAt(i);
	return sum;
}

}} // namespace flux::tar
