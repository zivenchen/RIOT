/*
 * Copyright (C) 2015 INRIA
 * Copyright (C) 2015 Eistec AB
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       Crash handling functions
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <string.h>
#include <stdio.h>

#include "cpu.h"
#include "irq.h"
#include "lpm.h"
#include "panic.h"
#include "arch/panic_arch.h"

#if DEVELHELP && defined MODULE_PS
#include "ps.h"
#endif

/* flag preventing "recursive crash printing loop" */
static int crashed = 0;

/* WARNING: this function NEVER returns! */
NORETURN void core_panic(int crash_code, const char *message)
{
    if (crashed == 0) {
        /* print panic message to console (if possible) */
        crashed = 1;
        puts("*** RIOT kernel panic");
        puts(message);
#if DEVELHELP
#ifdef MODULE_PS
        ps();
        puts("");
#endif

        puts("*** halted.\n");
#else
        puts("*** rebooting...\n\n");
#endif
    }
    /* disable watchdog and all possible sources of interrupts */
    disableIRQ();
    panic_arch();
#ifndef DEVELHELP
    /* DEVELHELP not set => reboot system */
    (void) reboot(RB_AUTOBOOT);
#endif

    /* tell the compiler that we won't return from this function
       (even if we actually won't even get here...) */
    UNREACHABLE();
}
