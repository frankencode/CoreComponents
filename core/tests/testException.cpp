/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/stdio>
#include <flux/check>
#include <flux/File>
#include <flux/Mutex>
#include <flux/exceptions>

using namespace flux;

int main()
{
    #ifndef NDEBUG
    try {
        Mutex::create()->release();
        check(false);
    }
    catch (Exception &ex) {
        fout() << ex << nl;
    }
    #endif

    try {
        String path = "testabc.123";
        if (!File::tryOpen(path)) FLUX_SYSTEM_DEBUG_ERROR(errno);
    }
    catch (Exception &ex) {
        fout() << ex << nl;
    }

    try {
        File::open("non-existing");
        return 1;
    }
    catch (Exception &ex) {
        fout() << ex << nl;
    }

    return 0;
}
