/*
 * Copyright (C) 2019 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <linux/futex.h>

inline static int futex_syscall(uint32_t *futex, int futex_op, int value)
{
    return syscall(SYS_futex, futex, futex_op, value, NULL, NULL, 0);
}

inline static void futex_acquire(uint32_t *futex)
{
    while (1) {
        if (__sync_bool_compare_and_swap(futex, 0, 1)) break;

        if (futex_syscall(futex, FUTEX_WAIT, 0) == -1) {
            if (errno != EAGAIN)
                exit(EXIT_FAILURE);
        }
    }
}

inline static void futex_release(uint32_t *futex)
{
    if (__sync_bool_compare_and_swap(futex, 1, 0)) {
        if (futex_syscall(futex, FUTEX_WAKE, 1) == -1)
            exit(EXIT_FAILURE);
    }
}
