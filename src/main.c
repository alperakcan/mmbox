/*
    Copyright (C) 2002  Alper Akcan

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    distch@hotmail.com
    icq : 651561
    tel : 0532 414 05 12
*/
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <signal.h>
#include <vga.h>
#include <vgagl.h>
#include <vgamouse.h>

#ifdef EMBEDDED
#include <sys/mount.h>
#endif

#include "main.h"
#include "kbd.h"
#include "mouse.h"
#include "utils.h"

#include "libirman/ir.h"
#include "jbox/jbox.h"
#include "jbox/updater.h"
#include "audio.h"
#include "clock.h"

extern int quit;

// mouse pressed rutin
void do_handler_case (int x, int y, int w, int h, char *img, char *box, int type);
char btn_box[30*30*2];
// signal functions
void cdstop (int foo);
void cdplisthandler (int foo);
void mp3stop (int foo);
void mp3plisthandler (int foo);
void updater_verbose_onoff (int foo);
void posbar_verbose_onoff (int foo);
void sanalyzer_change_color (int foo);

#ifndef EMBEDDED
volatile sig_atomic_t vrb_updtr_closed = 0;
volatile sig_atomic_t vrb_pbr_closed = 0;

void goto_back (void)
{
    if (!updtr_verbose) {
        updater_verbose_onoff(0);
	if (updaterpid>0)
    	    kill(updaterpid, SIG_UPDTR_VERBOSE);
	vrb_updtr_closed = 1;
    } else
	vrb_updtr_closed = 0;
    if (!posbar_verbose) {
	posbar_verbose_onoff(0);
	if (updaterpid>0)
	    kill(updaterpid, SIG_PSBR_VERBOSE);
	vrb_pbr_closed = 1;
    } else
	vrb_pbr_closed = 0;
}

void comefrom_back (void)
{
    if (vrb_updtr_closed) {
        if (updtr_verbose) {
    	    updater_verbose_onoff(0);
	    if (updaterpid>0)
    		kill(updaterpid, SIG_UPDTR_VERBOSE);
	}
	vrb_updtr_closed = 0;
    }
    if (vrb_pbr_closed) {
        if (posbar_verbose) {
	    posbar_verbose_onoff(0);
	    if (updaterpid>0)
		kill(updaterpid, SIG_PSBR_VERBOSE);
	}
	vrb_pbr_closed = 0;
    }
}
#endif

int mainhandler (void)
{
    int bhvr = 0;
    switch (key) {
        case RK_ESC : //quit
	    bhvr = 11; break;
	case 'i' : // irman set
	    bhvr = 5; break;
    }
    switch (ir) {
	case RI_QUIT:
	    bhvr = 11; break;
	    break;
    }
    // quit
    if_mouse_pressed_over(544, 454, 24, 24, MOUSE_LEFTBUTTON)
	bhvr = 1;
    // ir
    if_mouse_pressed_over(520, 454, 24, 24, MOUSE_LEFTBUTTON)
	bhvr = 5;
    // clock
    if_mouse_pressed_over(577, 456, 100, 23, MOUSE_RIGHTBUTTON)
	bhvr = 8;
    switch (bhvr) {
	case 1 : // quit with ask
	    do_handler_case(544, 454, 26, 26, NULL, btn_box, 1);
    	    if (updaterpid>0)
        	kill(updaterpid, SIG_UPDTR_VERBOSE);
	    updater_verbose_onoff(0);
    	    sleep_nano((long)50000000);
	    if(question_box(TEXT_QUEST, TEXT_QUITALL)) {
		quit = TRUE;
		jbox.uninit();
	    }
    	    if (updaterpid>0)
		kill(updaterpid, SIG_UPDTR_VERBOSE);
	    updater_verbose_onoff(0);
	    break;
	case 11 : // quit
	    quit = TRUE; jbox.uninit(); break;
	    break;
	case 5 : // irman set
	    do_handler_case(520, 454, 24, 24, NULL, btn_box, 1);
	    if (updaterpid>0)
	        kill(updaterpid, SIG_UPDTR_VERBOSE);
	    updater_verbose_onoff(0);
	    sleep_nano((long)50000000);
	    ir_setcommands();
	    if (updaterpid>0)
		kill(updaterpid, SIG_UPDTR_VERBOSE);
	    updater_verbose_onoff(0);
	    break;
	case 8 : // clock
    	    if (updaterpid>0)
        	kill(updaterpid, SIG_UPDTR_VERBOSE);
	    updater_verbose_onoff(0);
	    sleep_nano((long)50000000);
	    clock_set_time();
    	    if (updaterpid>0)
        	kill(updaterpid, SIG_UPDTR_VERBOSE);
	    updater_verbose_onoff(0);
	    break;
    }
}
    
int main (void)
{
#ifdef INSMOD
    int p, s;
    p = vfork();
    if (p == 0) {
	execl("/sbin/busybox", "insmod", "/sbin/svgalib_helper.o", 0);
	_exit(0);
    }
    sleep(1);
    kill(p, 9);
    waitpid(p, &s, 0);	
#endif

#ifdef EMBEDDED
    p = vfork();
    if (p == 0) {
	execl("/sbin/busybox", "mount", "-w", "-v", "-n",  "-o", "remount", "/", 0);
	_exit(0);
    }
    sleep(1);
    kill(p, 9);
    waitpid(p, &s, 0);
    mount("/dev/ram0", "/usr/local/share/tetromino/files", "ramfs", 0, NULL);
#endif
    rcfile_load();
    
    if (cfg.irman_device != NULL)
        if (irman_init() != 1) {
	    irman_uninit();
//	    main();
	}

    vga_disabledriverreport();
#ifdef EMBEDDED
    vga_setchipset(17);
#endif
    vga_init();
//    vga_lockvc();

    sigignore(18);
    sigignore(15);
    sigignore(2);

#ifndef EMBEDDED
    vga_runinbackground(VGA_GOTOBACK, goto_back);
    vga_runinbackground(VGA_COMEFROMBACK, comefrom_back);
#endif

    vga_setmode(MODE);
    gl_setcontextvga(MODE);
    gl_setrgbpalette();

    vga_setmousesupport(1);
//    mouse_setscale(37);
    mouse_setposition(WIDTH/2, HEIGHT/2);
    mouse_setxrange(0, WIDTH-1);
    mouse_setyrange(0, HEIGHT-1);
    mouse_setwrap(MOUSE_NOWRAP);

    // setting signal catchers
    sigset(SIG_CDKILL, cdstop);
    sigset(SIG_CDPLIST, cdplisthandler);
    sigset(SIG_MP3KILL, mp3stop);
    sigset(SIG_MP3PLIST, mp3plisthandler);
    sigset(SIG_UPDTR_VERBOSE, updater_verbose_onoff);
    sigset(SIG_PSBR_VERBOSE, posbar_verbose_onoff);
    sigset(SIG_SANLYZR_COLOR, sanalyzer_change_color);

    dsp_init();
    dsp_set(2, 44100);

    srand(time(NULL));
    
    _showbmpfile(IMG_TASKBAR, 0, 450, NULL, NULL, NULL, NULL);

    clock_init();

    handler.main_handler = mainhandler;
    handler.window_handler = jbox_handler;

    while (!quit) {
	UPDATERS(1);
	handler.main_handler();
	handler.window_handler();	
    }
    
    rcfile_save();
    irman_uninit();
    vga_setmode(TEXT);
    exit(0);
}
