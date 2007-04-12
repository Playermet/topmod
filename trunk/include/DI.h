/*
 * DI.h -  support for invariants (assertions) using the gdb debugger.
 *
 * Copyright (c) 1997 Phil Maker
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: DI.h,v 1.1 2006/09/04 19:22:52 stuart Exp $
 */

#ifndef _DI_h_
#define _DI_h_ 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WITHOUT_NANA 

/*
 * nana-config.h - the system wide configuration file; we put the ifndef
 *   around it to avoid the file 5 million times during a compile.
 */

#ifndef _nana_config_h_
#include <nana-config.h>
#endif

/* 
 * DI_LEVEL sets the level of invariant analogously to NDEBUG in assert.h
 *
 *   DI_LEVEL == 2: invariants are always evaluated.
 *   DI_LEVEL == 1: evaluate invariants iff they have a true GUARD.
 *   DI_LEVEL == 0: invariants are never evaluated.
 */

#ifndef DI_LEVEL /* define DEFAULT for DI_LEVEL */
#define DI_LEVEL 1
#endif



/*
 * DI_DEFAULT_GUARD - the default guard expression; an invariant is checked
 *     iff the guard is true. By default its always true.
 */

#ifndef DI_DEFAULT_GUARD
#define DI_DEFAULT_GUARD 1
#endif

/*
 * DI_DEFAULT_PARAMS - the default value to be passed as the second argument
 *       to the handler macro when an invariant fails.
 */

#ifndef DI_DEFAULT_PARAMS
#define DI_DEFAULT_PARAMS /* nothing */
#endif

/*
 * DI_DEFAULT_HANDLER - called when an error is detected by DI; 
 */

#ifndef DI_DEFAULT_HANDLER /* define default handler */

#define DI_DEFAULT_HANDLER(e,f,l,p) \
  @@echo e has failed at f:l with p\n@@ \
  @@where@@ /* stack backtrace */

#endif /* DI_DEFAULT_HANDLER */

/*
 * DI_MAKE_VALID_BREAKPOINT(e) - called whenever we generate a DI breakpoint;
 *     the aim is to make sure a breakpoint can be set at the current location
 *     and that any expressions will be evaluated by gdb correctly. 
 *     This is the portable default; an architecture specific version
 *     is generated by the configure script into nana-config.h
 */

#ifndef DI_MAKE_VALID_BREAKPOINT
static volatile int _di_target;

#define DI_MAKE_VALID_BREAKPOINT(e) _di_target = 0
#endif

/*
 * _DIGHPS - implements the DI macros, it comes in two variants:
 * 
 * ifdefined(_NANA_FILTER_) then we are generating debugger commands
 * else generating C text.
 */

#if DI_LEVEL == 2 /* always check the assertion */
#ifdef _NANA_FILTER_
#define _DIGHPS(e,g,h,p,s) \
	@@break @__FILE__:__LINE__@@ \
	@@condition $bpnum (!(e))@@ \
	@@command $bpnum@@ \
	@@silent@@ \
        h(s,f,l,p) \
	@@end@@
#else
#define _DIGHPS(e,g,h,p,s) DI_MAKE_VALID_BREAKPOINT(e)
#endif /* _NANA_FILTER_ */

#elif DI_LEVEL == 1 /* check it iff g is true */

#ifdef _NANA_FILTER_
#define _DIGHPS(e,g,h,p,s) \
	@@break @__FILE__:__LINE__@@ \
	@@condition $bpnum (g) && (!(e))@@ \
	@@command $bpnum@@ \
	@@silent@@ \
        h(s,f,l,p) \
	@@end@@
#else 
#define _DIGHPS(e,g,h,p,s) DI_MAKE_VALID_BREAKPOINT(e)
#endif /* _NANA_FILTER_ */

#elif DI_LEVEL == 0 /* no assertions so just remove them */
#define _DIGHPS(e,g,h,p,s) /* nothing */
#endif /* DI_LEVEL */


/*
 * DSG(e,g), DS(e) - are used to set variables in the debugger from 
 *        within C programs. These convenience variables can then be
 *        used in invariants later on to refer to the previous state 
 *        of the program.
 * 
 *        DS($x = x); ....; DI($x + 10 == x); 
 */

#if DI_LEVEL == 2
#ifdef _NANA_FILTER_
#define DSG(e,g) \
	@@break @__FILE__:__LINE__@@ \
	@@command@@ \
	@@silent@@ \
	@@set e@@ \
	@@cont@@ \
	@@end@@
#else 
#define DSG(e,g) DI_MAKE_VALID_BREAKPOINT(e)
#endif

#elif DI_LEVEL == 1
#ifdef _NANA_FILTER_
#define DSG(e,g) \
	@@break @__FILE__:__LINE__ if g@@ \
	@@command@@ \
	@@silent@@ \
	@@set e@@ \
	@@cont@@ \
	@@end@@
#else
#define DSG(e,g) DI_MAKE_VALID_BREAKPOINT(e)
#endif /* _NANA_FILTER_ */

#elif DI_LEVEL == 0
#define DSG(e,g) /* nothing */
#else
error DI_LEVEL should be 0 or 1 or 2
#endif

#define DS(e) DSG(e,DI_DEFAULT_GUARD)

/*
 * And all the user macros; these are used to put in the default arguments.
 * The name is used to determine the arguments; e.g. DIGH takes an expression
 * to check; a guard and a handler as parameters. The letters in the names
 * are in ascending order (i.e. DIGH(...) not DIHG(...)).
 *
 * DI[G][H][P] - it must be true (e) with an optional guard, handler and 
 *    parameter for the handler.
 * DN[G][H][P] - as for DI... except that (e) must never ever be true.
 */

#define DI(e) \
  _DIGHPS(e,DI_DEFAULT_GUARD,DI_DEFAULT_HANDLER,DI_DEFAULT_PARAMS,"DI("#e")")
#define DIG(e,g) \
  _DIGHPS(e,g,DI_DEFAULT_HANDLER,DI_DEFAULT_PARAMS,"DI("#e")")
#define DIH(e,h) \
  _DIGHPS(e,DI_DEFAULT_GUARD,h,DI_DEFAULT_PARAMS,"DI("#e")")
#define DIP(e,p) \
  _DIGHPS(e,DI_DEFAULT_GUARD,DI_DEFAULT_HANDLER,p,"DI("#e")")
#define DIGH(e,g,h) \
  _DIGHPS(e,g,h,DI_DEFAULT_PARAMS,"DI("#e")")
#define DIGP(e,g,p) \
  _DIGHPS(e,g,h,p,"DI("#e")")
#define DIHP(e,h,p) \
  _DIGHPS(e,DI_DEFAULT_GUARD,h,p,"DI("#e")")
#define DIGHP(e,g,h,p) \
  _DIGHPS(e,g,h,p,"DI("#e")")

#define DN(e) \
  _DIGHPS((!(e)),DI_DEFAULT_GUARD,DI_DEFAULT_HANDLER,DI_DEFAULT_PARAMS,"DN("#e")")
#define DNG(e,g) \
  _DIGHPS((!(e)),g,DI_DEFAULT_HANDLER,DI_DEFAULT_PARAMS,"DN("#e")")
#define DNH(e,h) \
  _DIGHPS((!(e)),DI_DEFAULT_GUARD,h,DI_DEFAULT_PARAMS,"DN("#e")")
#define DNP(e,p) \
  _DIGHPS((!(e)),DI_DEFAULT_GUARD,DI_DEFAULT_HANDLER,p,"DN("#e")")
#define DNGH(e,g,h) \
  _DIGHPS((!(e)),g,h,DI_DEFAULT_PARAMS,"DN("#e")")
#define DNGP(e,g,p) \
  _DIGHPS((!(e)),g,h,p,"DN("#e")")
#define DNHP(e,h,p) \
  _DIGHPS((!(e)),DI_DEFAULT_GUARD,h,p,"DN("#e")")
#define DNGHP(e,g,h,p) \
  _DIGHPS((!(e)),g,h,p,"DN("#e")")

#else /* defined(WITHOUT_NANA) */

#define DI(e) /* empty */
#define DIG(e,g) /* empty */
#define DIH(e,h) /* empty */
#define DIP(e,p) /* empty */
#define DIGH(e,g,h) /* empty */
#define DIGP(e,g,p) /* empty */
#define DIHP(e,h,p) /* empty */
#define DIGHP(e,g,h,p) /* empty */

#define DN(e) /* empty */
#define DNG(e,g) /* empty */
#define DNH(e,h) /* empty */
#define DNP(e,p) /* empty */
#define DNGH(e,g,h) /* empty */
#define DNGP(e,g,p) /* empty */
#define DNHP(e,h,p) /* empty */
#define DNGHP(e,g,h,p) /* empty */

#define DS(e) /* empty */
#define DSG(e,g) /* empty */


#endif /* !defined(WITHOUT_NANA) */
#ifdef __cplusplus
}
#endif

#endif /* _DI_h_ */



