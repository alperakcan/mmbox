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
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <vgamouse.h>

#include "jbox.h"
#include "../main.h"
#include "../audio.h"
#include "../utils.h"
#include "../mouse.h"
#include "../kbd.h"
#include "../font.h"
#include "../libirman/ir.h"
#include "../dirbrowser.h"
#include "cd.h"
#include "mp3.h"
#include "sanalyzer.h"
#include "updater.h"
#include "playlist.h"
#include "games/games.h"

extern int plpart;
static int first_time = 0;

#define MP3BOX 1
#define CDBOX 2

static int box = 0;
static paused_btn_box[50*50*2]; // as caglar wanted

void jbox_init (void)
{
    _showbmpfile(IMG_JBOX_TOP, 0, 0, NULL, NULL, NULL, NULL);
    _showbmpfile(IMG_JBOX_LEFT, 0, 26, NULL, NULL, NULL, NULL);
    _showbmpfile(IMG_JBOX_BTM, 56, 394, NULL, NULL, NULL, NULL);
    _showbmpfile(IMG_JBOX_RIGHT, 556, 26, NULL, NULL, NULL, NULL);
    _showbmpfile(IMG_JBOX_HSPRT, 267, 26, NULL, NULL, NULL, NULL);
    _showbmpfile(IMG_JBOX_LOGO, /*48*/82, /*29*/31, NULL, NULL, NULL, NULL);
    _showbmpfile(IMG_JBOX_VSPRT, 43, 107, NULL, NULL, NULL, NULL);
    _showbmpfile(IMG_JBOX_ANLYZR, 120, 112, NULL, NULL, NULL, NULL);
    _showbmpfile(IMG_JBOX_VSPRT, 43, 220, NULL, NULL, NULL, NULL);
    _showbmpfile(IMG_JBOX_TITLE, 120, 223, NULL, NULL, NULL, NULL);
    gl_getbox(301, 422, 246, 18, posbar);
    gl_getbox(6, 284, 50, 50, paused_btn_box);
}

int jbox_handler (void)
{
    int sw, i, bhvr = 0, pb = 0;
    int is_over = 0;
    char btn_box[50*50*2];
    
    if (!first_time) {
	jbox_init();
	mixer_set(70);
	mp3boxinit();
	box = MP3BOX;
	first_time = 1;
    }
    // mouse handler
    // mp3 box
    if_mouse_pressed_over(7, 13, 35, 30, MOUSE_LEFTBUTTON)
	bhvr = 1;
    // cd box
    if_mouse_pressed_over(7, 42, 35, 30, MOUSE_LEFTBUTTON)
	bhvr = 2;
    // video box
    if_mouse_pressed_over(7, 72, 35, 30, MOUSE_LEFTBUTTON)
	bhvr = 3;
    // tv & radio box
    if_mouse_pressed_over(5, 102, 35, 30, MOUSE_LEFTBUTTON)
	bhvr = 4;
    // games
    if_mouse_over(7, 134, 29, 29) {
	gl_getbox(6, 168, 31, 61, btn_box);
	_showbmpfile(IMG_JBOX_BTN_TETRISMINE, 6, 168, NULL, NULL, NULL, NULL);
	is_over = 1;
	while (is_over) {
	    UPDATERS(1);
	    if_mouse_over(7, 134, 29, 95)
		is_over = 1;
	    else {
		is_over = 0;
		gl_putbox(6, 168, 31, 61, btn_box);
	    }
	    if_mouse_pressed_over(6, 168, 29, 29, MOUSE_LEFTBUTTON) {
		bhvr = 51; // tetris
		is_over = 0;
		gl_putbox(6, 168, 31, 61, btn_box);
	    }
	    if_mouse_pressed_over(6, 199, 29, 29, MOUSE_LEFTBUTTON) {
		bhvr = 52; // mine
		is_over = 0;
		gl_putbox(6, 168, 31, 61, btn_box);
	    }
	}
    }
    if_mouse_pressed_over(6, 284, 50, 50, MOUSE_LEFTBUTTON)
	bhvr = 6;
    // pause
    if_mouse_pressed_over(6, 284, 50, 50, MOUSE_LEFTBUTTON)
	bhvr = 6;
    // stop
    if_mouse_pressed_over(7, 340, 50, 50, MOUSE_LEFTBUTTON)
	bhvr = 7;
    // play
    if_mouse_pressed_over(7, 394, 50, 50, MOUSE_LEFTBUTTON)
	bhvr = 8;
    // prev
    if_mouse_pressed_over(64, 394, 50, 50, MOUSE_LEFTBUTTON)
	bhvr = 9;
    // next
    if_mouse_pressed_over(122, 394, 50, 50, MOUSE_LEFTBUTTON)
	bhvr = 10;
    // vol down 5
    if_mouse_pressed_over(579, 424, 46, 12, MOUSE_LEFTBUTTON)
	bhvr = 111;
    // vol down 10
    if_mouse_pressed_over(579, 411, 46, 12, MOUSE_LEFTBUTTON)
	bhvr = 112;
    // vol up 10
    if_mouse_pressed_over(579, 398, 46, 12, MOUSE_LEFTBUTTON)
	bhvr = 113;
    // vol up 5
    if_mouse_pressed_over(579, 386, 46, 12, MOUSE_LEFTBUTTON)
	bhvr = 114;
    // cd
    if_mouse_pressed_over(604, 324, 36, 32, MOUSE_LEFTBUTTON) {
	if (box == MP3BOX)
	    bhvr = 120;
	else if (box == CDBOX)
	    bhvr = 121;
    }
    // cd select mp3 box
    if_mouse_pressed_over(604, 324, 36, 32, MOUSE_RIGHTBUTTON)
	bhvr = 1200;
    // repeat
    if_mouse_pressed_over(604, 289, 36, 36, MOUSE_LEFTBUTTON)
	bhvr = 13;
    // shuffle
    if_mouse_pressed_over(604, 251, 36, 36, MOUSE_LEFTBUTTON)
	bhvr = 14;
    // sanalyzer color
    if_mouse_pressed_over(49, 213-73, 224, 73, MOUSE_LEFTBUTTON)
	bhvr = 15;
    // set position bar
    if_mouse_pressed_over(301, 422, 246, 18, MOUSE_LEFTBUTTON)
	bhvr = 16;

    // keyboard & IR handler
    for (sw=key, i=0; i<2; i++, sw=ir) {
        switch (sw) {
	    case RK_F1 : case RI_JBOXMP3 :
		bhvr = 1; break;
	    case RK_F2 : case RI_JBOXCD :
		bhvr = 2; break;
	    case 'v': case RI_PREV :
		bhvr = 9; break;
	    case 'c' : case RI_PLAY :
		bhvr = 8; break;
	    case 'z' : case RI_PAUSE :
		bhvr = 6; break;
	    case 'x' : case RI_STOP :
		bhvr = 7; break;
	    case 'b' : case RI_NEXT :
		bhvr = 10; break;
	    case 's' : case RI_SHUFFLE :
		bhvr = 14; break;
	    case 'r' : case RI_REPLAY :
		bhvr = 13; break;
	    case RK_CURSOR_UP : case RI_VOL_UP :
		bhvr = 113; break;
	    case RK_CURSOR_DOWN : case RI_VOL_DOWN :
		bhvr = 112; break;
	    case 't' :
		bhvr = 51; break;
	    case 'm' :
		bhvr = 52; break;
	    case 'e' :
		if (box == MP3BOX)
		    bhvr = 120;
		else if (box == CDBOX)
		    bhvr = 121;
		break;
	}
    }
    // do bhvr
    switch (bhvr) {
	case 1 : // mp3
	    do_handler_case(7, 13, 35, 31, NULL, btn_box, 1);
	    if (box != MP3BOX) {
	        jbox.uninit();
	        mp3boxinit();
	        box = MP3BOX;
	    }
	    break;
	case 2 : // cd
	    do_handler_case(7, 42, 35, 33, NULL, btn_box, 1);
	    if (box != CDBOX) {
	        jbox.uninit();
	        cdboxinit();
	        box = CDBOX;
	    }
	    break;
	case 3 : // video
	    do_handler_case(7, 72, 35, 33, NULL, btn_box, 1);
	    break;
	case 4 : // radio & tv
	    do_handler_case(5, 102, 35, 33, NULL, btn_box, 1);
	    break;
	case 5 : // game
	case 51 : // tetris
	    if (game_minimized)
		return -1;
            if (updaterpid>0)
	        kill(updaterpid, SIG_UPDTR_VERBOSE);
	    updater_verbose_onoff(0);
	    sleep_nano((long)50000000);
	    game_tetris();
	    if (updaterpid>0)
		kill(updaterpid, SIG_UPDTR_VERBOSE);
	    updater_verbose_onoff(0);
	    break;
	case 52 : // minesweeper
	    if (game_minimized)
		return -1;
	    if (updaterpid>0)
	        kill(updaterpid, SIG_UPDTR_VERBOSE);
	    updater_verbose_onoff(0);
	    sleep_nano((long)50000000);
	    game_minesweeper();
	    if (updaterpid>0)
		kill(updaterpid, SIG_UPDTR_VERBOSE);
	    updater_verbose_onoff(0);
	    break;
	case 6 : // pause
	    do_handler_case(6, 284, 50, 50, IMG_JBOX_BTN_PAUSE, btn_box, 0);
	    jbox.pause();
	    break;
	case 7 : // stop
	    do_handler_case(7, 340, 50, 50, IMG_JBOX_BTN_STOP, btn_box, 0);
	    jbox.stop(0);
	    break;
	case 8 : // play
	    do_handler_case(7, 394, 50, 50, IMG_JBOX_BTN_PLAY, btn_box, 0);
	    jbox.play(0);
	    break;
	case 9 : // prev
	    do_handler_case(64, 394, 50, 50, IMG_JBOX_BTN_PREV, btn_box, 0);
	    jbox.prev();
	    break;
	case 10 : // next
	    do_handler_case(122, 394, 50, 50, IMG_JBOX_BTN_NEXT, btn_box, 0);
	    jbox.next();
	    break;
	case 111 : // volme down 1
	    do_handler_case(579, 424, 46, 12, IMG_JBOX_BTN_VOLD5, btn_box, 0);
	    vol_current -= 1;
	    mixer_set(vol_current);
	    break;
	case 112 : // volume down 5
	    do_handler_case(579, 411, 46, 12, IMG_JBOX_BTN_VOLD10, btn_box, 0);
	    vol_current -= 5;
	    mixer_set(vol_current);
	    break;
	case 113 : // volme up 5
	    do_handler_case(579, 398, 46, 12, IMG_JBOX_BTN_VOLU10, btn_box, 0);
	    vol_current += 5;
	    mixer_set(vol_current);
	    break;
	case 114 : // volume up 1
	    do_handler_case(579, 386, 46, 12, IMG_JBOX_BTN_VOLU5, btn_box, 0);
	    vol_current += 1;
	    mixer_set(vol_current);
	    break;
	case 120 : // we are in mp3 box, this will mount choosen device
	    do_handler_case(604, 324, 36, 34, NULL, btn_box, 1);
	    cd_mount();
	    break;
	case 1200 :
	    cd_select_device();
	    break;
	case 121 : // we are in cd box, this will init choosen device
	    do_handler_case(604, 324, 36, 34, NULL, btn_box, 1);
	    cdinfo();
	    break;
	case 13 : // repeat
	    do_handler_case(604, 289, 36, 38, NULL, btn_box, 1);
	    jbox.replay();
	    break;
	case 14 : // shuffle
	    do_handler_case(604, 251, 36, 38, NULL, btn_box, 1);
	    jbox.shuffle();
	    break;
	case 15 : // sanalyzer color
	    if (updaterpid>0)
	        kill(updaterpid, SIG_SANLYZR_COLOR);
	    _mouse_waitbuttonrealese();
	    sanalyzer_change_color(0);
	    break;
	case 16 : // set position bar
	    pb = ((mouse.x - 301)*100)/245;
	    _mouse_waitbuttonrealese();
	    jbox.play(pb);
	    break;	    
    }
    // playlist handler
    jbox.playlist_handler(key);
    return 0;
}

int on_off (int *i)
{
    if (!(*i))
	*i = 1;
    else
	*i = 0;
}

int boxpause (int *s, int *p)
{
    switch (*s) {
	case PLAYING :
	    kill(*p, SIGSTOP);
	    *s = PAUSED;
	    _showbmpfile(IMG_JBOX_BTN_PAUSE, 6, 284, NULL, NULL, NULL, NULL);
	    break;
	case PAUSED :
	    kill(*p, SIGCONT);
	    *s = PLAYING;
	    gl_putbox(6, 284, 50, 50, paused_btn_box);
	    break;
    }
}

int boxprev (int *num, int shuffle, int replay, int first, int last)
{
    if (shuffle == 0) {
	if (replay == 0)
	    if ((*num)>first)
	        (*num)--;
	    else
		return 0;
	if (replay == 1) {
	    (*num)--;
	    if ((*num)<first)
	        (*num) = last;
	}
    }
    if (shuffle == 1)
	(*num) = _rand(last);
    return 1;
}

int boxnext (int *num, int shuffle, int replay, int first, int last)
{
    if (shuffle == 0) {
	if (replay == 0)
	    if ((*num)<last)
	        (*num)++;
	    else
		return 0;
	if (replay == 1) {
	    (*num)++;
	    if ((*num)>last)
	        (*num) = first;
	}
    }
    if (shuffle == 1)
	(*num) = _rand(last);
    return 1;
}

int boxstop (int *stat, int *p, int *up)
{
    int s;
    
    if((*stat) == PAUSED)
	jbox.pause();
    
    if (((*stat) == PLAYING && (*p) != 0) || ((*p) != 0)) {
        kill((*p), SIGCONT);
	kill((*p), 9);
	waitpid((*p), &s, 0);
	(*p) = 0;
        kill((*up), SIGCONT);
	kill((*up), 9);
	waitpid((*up), &s, 0);
	(*up) = 0;
	updaterpid = 0;

        sanalyzer_uninit();
	(*stat) = STOPPED;
    }
    sanalyzer_clear();
    updater_title_clear();
}

void draw_shufrep(int s, int r)
{
    int ks, kr, c;
    if (s)
	c = gl_rgbcolor(128, 128, 128);
    else
	c = gl_rgbcolor(70, 70, 70);
    ks = _drawtext(62, 380, c, helvR08, TEXT_SHUFFLE, 208, 23, FONTNOSCROLL, 0);
    _drawtext(62+208-ks, 380, c, helvR08, TEXT_SHUFFLE, 208, 23, FONTNOSCROLL, 1);
    if (r)
	c = gl_rgbcolor(128, 128, 128);
    else
	c = gl_rgbcolor(70, 70, 70);
    ks = _drawtext(62, 395, c, helvR08, TEXT_REPEAT, 208, 23, FONTNOSCROLL, 0);
    _drawtext(62+208-ks, 395, c, helvR08, TEXT_REPEAT, 208, 23, FONTNOSCROLL, 1);
}

void do_handler_case (int x, int y, int w, int h, char *img, char *box, int type)
{
    gl_getbox(x, y, w, h, box);
    if (type == 0)
        _showbmpfile(img, x, y, NULL, NULL, NULL, NULL);
    if (type == 1) {
	gl_putbox(x+1, y+2, w, h-2, box);
	draw_button_area31(x, y, w-1, h-1);
    }
    _mouse_waitbuttonrealese();
    gl_putbox(x, y, w, h, box);
}
