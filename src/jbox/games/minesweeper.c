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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <vga.h>
#include <vgagl.h>
#include <vgamouse.h>

#include "games.h"
#include "../jbox.h"
#include "../updater.h"
#include "../../utils.h"
#include "../../font.h"
#include "../../mouse.h"
#include "../../main.h"

#define W 16
#define H 16
#define GAME_AREA_W (W*20+15)
#define GAME_AREA_H (H*20+15)
#define INST_AREA_W (220)
#define INST_AREA_H (220)
#define GAME_AREA_X (((640-GAME_AREA_W-INST_AREA_W)/2)-20)
#define GAME_AREA_Y (480-GAME_AREA_H)/2
#define INST_AREA_X (GAME_AREA_X+GAME_AREA_W+10)
#define INST_AREA_Y GAME_AREA_Y
#define MINES 40

static int screen_matrix[H][W];
static int quest_matrix[H][W];
static int opened_matrix[H][W];
static int game_finished = 0;

void show_nulls (void)
{
    //
}

int quest_bomb (int x, int y)
{
    if (quest_matrix[y][x] == 1) {
	quest_matrix[y][x] = 2;
	return 2;
    }
    if (quest_matrix[y][x] == 2) {
	quest_matrix[y][x] = 0;
	return 0;
    } else if (quest_matrix[y][x] == 0) {
	quest_matrix[y][x] = 1;
	return 1;
    } else
	return -1;
}

void mines_init (void)
{
    int i;
    int m, mx, my;
    int x, y;
    
    for (y=0; y<H; y++)
	for (x=0; x<W; x++) {
	    screen_matrix[y][x] = 0;
	    quest_matrix[y][x] = 0;
	    opened_matrix[y][x] = 0;
	}
    srand(time(NULL));
    for (i=0; i<MINES; i++) {
        m = _rand(H*W);
	my = m/W; mx = m%W;
	if (screen_matrix[my][mx] == -1) {
	    i--;
	    continue;
	}
	else 
	    screen_matrix[my][mx] = -1;
    }
    // calcs matrix num
    for (y=0; y<H; y++)
	for (x=0; x<W; x++) {
	    i = 0;
	    // top
	    if (y>0) 
		if (screen_matrix[y-1][x] == -1)
	    	    i++;
	    // top left
	    if ((y>0) && (x>0))
		if (screen_matrix[y-1][x-1] == -1)
	    	    i++;
	    // left
	    if (x>0)
		if(screen_matrix[y][x-1] == -1)
		    i++;
	    // buttom left
	    if ((y<(H-1)) && (x>0))
		if (screen_matrix[y+1][x-1] == -1)
		    i++;
	    // buttom
	    if (y<(H-1))
		if(screen_matrix[y+1][x] == -1)
	    	    i++;
	    // buttom right
	    if ((y<(H-1)) && (x<(W-1)))
		if(screen_matrix[y+1][x+1] == -1)
	    	    i++;
	    // right
	    if (x<(W-1))
		if (screen_matrix[y][x+1] == -1)
	    	    i++;
	    // top right
	    if ((y>0) && (x<(W-1)))
		if (screen_matrix[y-1][x+1] == -1)
	    	    i++;
	    if(screen_matrix[y][x] == 0)
		screen_matrix[y][x] = i;
	}
}

#define mines_show_matrix(a)	draw_button_area1(GAME_AREA_X+10+20*x, GAME_AREA_Y+10+20*y, 20, 20);\
				switch (screen_matrix[y][x]) {\
				    case 1 : c = '1'; break;\
				    case 2 : c = '2'; break;\
				    case 3 : c = '3'; break;\
				    case 4 : c = '4'; break;\
				    case 5 : c = '5'; break;\
				    case 6 : c = '6'; break;\
				    case 7 : c = '7'; break;\
				    case 8 : c = '8'; break;\
				    case -1 :\
				        gl_fillbox(GAME_AREA_X+10+20*x+2, GAME_AREA_Y+10+20*y+2, 16, 16, gl_rgbcolor(255, 0, 0));\
					if (a) {\
					    for (y=0; y<H; y++)\
						for (x=0; x<W; x++)\
						    if (screen_matrix[y][x] == (-1)) {\
							draw_button_area1(GAME_AREA_X+10+20*x, GAME_AREA_Y+10+20*y, 20, 20);\
							gl_fillbox(GAME_AREA_X+10+20*x+2, GAME_AREA_Y+10+20*y+2, 16, 16, gl_rgbcolor(255, 0, 0));\
							opened_matrix[y][x] = 1;\
						}\
					    game_finished = 1;\
					}\
					break;\
				    case 0 : show_nulls(); break;\
				}\
				if ((c>='1') && (c<='8'))\
				    _drawtext(GAME_AREA_X+10+20*x+5, GAME_AREA_Y+10+20*y+2, 0, helvB10, &c, 10, 17, FONTNOSCROLL, 1);\
				c = '9';

void draw_game_area_minesweeper (char *buf, char *buf2, int m)
{
    int x, y;
    char c;
    
    gl_getbox(GAME_AREA_X, GAME_AREA_Y, GAME_AREA_W, GAME_AREA_H, buf);
    draw_button_area1(GAME_AREA_X, GAME_AREA_Y, GAME_AREA_W, GAME_AREA_H);
    gl_fillbox(GAME_AREA_X+5, GAME_AREA_Y+5, GAME_AREA_W-8, GAME_AREA_H-8, 0);
    // inst.. area
    gl_getbox(INST_AREA_X, INST_AREA_Y, INST_AREA_W, INST_AREA_H, buf2);
    draw_button_area1(INST_AREA_X, INST_AREA_Y, INST_AREA_W, INST_AREA_H);
    gl_fillbox(INST_AREA_X+5, INST_AREA_Y+5, INST_AREA_W-8, INST_AREA_H-8, 0);
    _drawtext(INST_AREA_X+15, INST_AREA_Y+15, gl_rgbcolor(128, 128, 128), helvB10, LEGEND, 180, 120, FONTNOSCROLL, 1);
    _drawtext(INST_AREA_X+30, INST_AREA_Y+40, gl_rgbcolor(128, 128, 128), helvB10, MINES_LEGEND, 180, 120, FONTNOSCROLL, 1);
    _drawtext(INST_AREA_X+15, INST_AREA_Y+185, gl_rgbcolor(128, 128, 128), helvR08, MINES_HINT, 180, 20, FONTNOSCROLL, 1);
    
    for (y=0; y<H; y++)
        for (x=0; x<W; x++)
	    draw_button_area0(GAME_AREA_X+10+20*x, GAME_AREA_Y+10+20*y, 20, 20);

    if (m) {
        mines_init();
    } else {
        for (y=0; y<H; y++)
	    for (x=0; x<W; x++) {
		if (opened_matrix[y][x] == 1) {
		    mines_show_matrix(0);
		}
		if (quest_matrix[y][x] != 0) {
		    switch (quest_matrix[y][x]) {
		        case 2 :
			    _drawtext(GAME_AREA_X+10+20*x+4, GAME_AREA_Y+10+20*y+2, gl_rgbcolor(255, 0, 0), helvB10, "?", 10, 17, FONTNOSCROLL, 1);
			    break;
			case 1 :
			    _drawtext(GAME_AREA_X+10+20*x+4, GAME_AREA_Y+10+20*y+2, gl_rgbcolor(255, 0, 0), helvB10, "B", 10, 17, FONTNOSCROLL, 1);
			    break;
		    }
		}
	    }
    }
}

#define draw_mines_matrix() draw_game_area_minesweeper(buf, buf2, 0)



void game_minesweeper (void)
{
    char *buf, *buf2, *buf3;
    int x, y, k;
    char c;

    buf = (char *) calloc(2, GAME_AREA_W * GAME_AREA_H);
    buf2 = (char *) calloc(2, INST_AREA_W * INST_AREA_H);
    buf3 = (char *) calloc(2, 50 * 20);
    draw_game_area_minesweeper(buf, buf2, 1);
    
    gl_getbox(220, 460, 50, 20, buf3);
    draw_button(220, 460, "Mines..", 1);
    
    game_finished = 0;
    
    while (1) {
	UPDATERS(1);
	games_minimize("", "Mines..", draw_mines_matrix(), 0);
        switch (key) {
	    case 'q' :
		quit_game(0);
		return;
	    case 'r' :
		quit_game(0);
		game_minesweeper();
		return;
        }
	if (game_finished == 1)
	    continue;
        for (y=0; y<H; y++)
	    for (x=0; x<W; x++) {
		if_mouse_pressed_over(GAME_AREA_X+10+20*x, GAME_AREA_Y+10+20*y, 20, 20, MOUSE_LEFTBUTTON) {
		    mines_show_matrix(1);
		    _mouse_waitbuttonrealese();
		    opened_matrix[y][x] = 1;
		}
		if_mouse_pressed_over(GAME_AREA_X+10+20*x, GAME_AREA_Y+10+20*y, 20, 20, MOUSE_RIGHTBUTTON) {
		    if (opened_matrix[y][x] == 0 ) {
			draw_button_area0(GAME_AREA_X+10+20*x, GAME_AREA_Y+10+20*y, 20, 20);
		        switch (quest_bomb(x, y)) {
			    case 2 :
				_drawtext(GAME_AREA_X+10+20*x+4, GAME_AREA_Y+10+20*y+2, gl_rgbcolor(255, 0, 0), helvB10, "?", 10, 17, FONTNOSCROLL, 1);
				break;
			    case 1 :
				_drawtext(GAME_AREA_X+10+20*x+4, GAME_AREA_Y+10+20*y+2, gl_rgbcolor(255, 0, 0), helvB10, "B", 10, 17, FONTNOSCROLL, 1);
				break;
			}			
			_mouse_waitbuttonrealese();
		    }
	        }
	    }
    }
}
