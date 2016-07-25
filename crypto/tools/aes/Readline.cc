/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <stdio.h>
#include <readline/readline.h>
#include <cc/stdio>
#include "Readline.h"

namespace ccaes {

static void password_redisplay()
{}

String Readline::getPassword(String prompt)
{
    rl_voidfunc_t * old_redisplay = rl_redisplay_function;
    rl_redisplay_function = password_redisplay;
    fout() << prompt;
    String text = readline("");
    rl_redisplay_function = old_redisplay;
    return text;
}

} // namespace ccaes
