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
// super macro ;)
// T is the text thas was on legend part
// A is the text that is written to the task bar button
// F restoring function
// B must be always 0, tetris needs 33.
#define games_minimize(T, A, F, B)	if_mouse_pressed_over(220, 460, 50, 19, MOUSE_LEFTBUTTON) {\
					    draw_button(220, 460, A, 0);\
					    _mouse_waitbuttonrealese();\
					    game_minimized = 1;\
					}\
					if (key == 'm') {\
					    draw_button(220, 460, A, 0);\
					    _mouse_waitbuttonrealese();\
					    game_minimized = 1;\
					}\
					if (game_minimized) {\
					    if (updaterpid>0)\
					        kill(updaterpid, SIG_UPDTR_VERBOSE);\
					    updater_verbose_onoff(0);\
					    gl_putbox(GAME_AREA_X, GAME_AREA_Y, GAME_AREA_W, GAME_AREA_H+B, buf);\
					    gl_putbox(INST_AREA_X, INST_AREA_Y, INST_AREA_W, INST_AREA_H, buf2);\
					    draw_button(220, 460, A, 0);\
					    while (game_minimized) {\
						UPDATERS(1);\
						if (jbox_handler() == -1) {\
						    draw_button(220, 460, A, 1);\
						    _mouse_waitbuttonrealese();\
						    if (updaterpid>0)\
							kill(updaterpid, SIG_UPDTR_VERBOSE);\
						    updater_verbose_onoff(0);\
						    sleep_nano((long)50000000);\
						    game_minimized = 0;\
						    F;\
						    _drawtext(INST_AREA_X+(INST_AREA_W-k)/2, INST_AREA_Y+185, gl_rgbcolor(255, 0, 0), helvB10, T, 180, 20, FONTNOSCROLL, 1);\
						}\
						if_mouse_pressed_over(220, 460, 50, 19, MOUSE_LEFTBUTTON) {\
						    draw_button(220, 460, A, 1);\
						    _mouse_waitbuttonrealese();\
						    if (updaterpid>0)\
							kill(updaterpid, SIG_UPDTR_VERBOSE);\
						    updater_verbose_onoff(0);\
						    sleep_nano((long)50000000);\
						    game_minimized = 0;\
						    F;\
						    _drawtext(INST_AREA_X+(INST_AREA_W-k)/2, INST_AREA_Y+185, gl_rgbcolor(255, 0, 0), helvB10, T, 180, 20, FONTNOSCROLL, 1);\
						}\
					    }\
					}
// a must be 33 for tetris otherwise 0.
#define quit_game(a)	gl_putbox(GAME_AREA_X, GAME_AREA_Y, GAME_AREA_W, GAME_AREA_H+a, buf);\
			gl_putbox(INST_AREA_X, INST_AREA_Y, INST_AREA_W, INST_AREA_H, buf2);\
			gl_putbox(220, 460, 50, 20, buf3);\
			free(buf);\
			free(buf2);\
			free(buf3)
int game_minimized;

void game_tetris (void);

void game_minesweeper (void);
