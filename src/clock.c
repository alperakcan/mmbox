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
#include <vgagl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vgagl.h>
#include <vgamouse.h>

#include "main.h"
#include "utils.h"
#include "font.h"
#include "mouse.h"
#include "fonts/_clockLCDfont.h"

#define DOCKCLOCK_X 577
#define DOCKCLOCK_Y 456

static char clockbox[58*19*2];

static char hourminutetmp[] = "....."; /* tmp to control the clock change */

void clock_init (void)
{
    gl_getbox(DOCKCLOCK_X, DOCKCLOCK_Y, 58, 19, clockbox);
}

void write_clock(int clockx, int clocky, char *clock)
{
    int i, y, x;
    char *LCD_FONT_NUMBER;

    /* hourminute variable will have 5 chars hh:mm,
     * we are going to draw clock number by number.*/
    for (i=0; i<5; i++)
    {
	switch(clock[i])
	{
	case '0':
	    LCD_FONT_NUMBER = LCD_FONT_0; break;
	case '1':
	    LCD_FONT_NUMBER = LCD_FONT_1; break;
	case '2':
	    LCD_FONT_NUMBER = LCD_FONT_2; break;
	case '3':
	    LCD_FONT_NUMBER = LCD_FONT_3; break;
	case '4':
	    LCD_FONT_NUMBER = LCD_FONT_4; break;
	case '5':
	    LCD_FONT_NUMBER = LCD_FONT_5; break;
	case '6':
	    LCD_FONT_NUMBER = LCD_FONT_6; break;
	case '7':
	    LCD_FONT_NUMBER = LCD_FONT_7; break;
	case '8':
	    LCD_FONT_NUMBER = LCD_FONT_8; break;
	case '9':
	    LCD_FONT_NUMBER = LCD_FONT_9; break;
	case ':':
	    LCD_FONT_NUMBER = LCD_FONT_DOT; break;
	}
	
	/* draw */
	for (y=0; y<19; y++)
	{
	    for (x=0; x<11; x++)
	    {
		if (*LCD_FONT_NUMBER!='#')
		{
		    switch(*LCD_FONT_NUMBER)
		    {
			case 'a': /* common color */
			    gl_setpixel(clockx+x, clocky+y, COL_CLOCK_C);
			    break;
			case 'b': /* shadow effect */
			    gl_setpixel(clockx+x, clocky+y, COL_CLOCK_S);
			    break;
		    }
		}
		LCD_FONT_NUMBER++;
	    }
	}
    clockx += 11;
    }
}

void clock_update()
{
    int i;
    long timer;
    struct tm *tinfo;
    char hourminute[5];
    
    timer = time(NULL);
    tinfo = localtime(&timer);
    sprintf(hourminute,"%02d:%02d", tinfo->tm_hour, tinfo->tm_min);
    /* control if minute has changed */
    if(strcmp(hourminute, hourminutetmp) != 0)
    {
	/* clean the area, with snapshot */
	gl_putbox(DOCKCLOCK_X, DOCKCLOCK_Y, 58, 19, clockbox);
	write_clock(DOCKCLOCK_X+2, DOCKCLOCK_Y, hourminute);
	/* copy it to temp for later controls.*/
	strcpy(hourminutetmp, hourminute);
    }
    /* for nice looking, sync dots with sec. */
    if((tinfo->tm_sec)%2) /* even secs will have dots */
	 gl_putbox(DOCKCLOCK_X+28, DOCKCLOCK_Y+3, 4, 14, clockbox);
    else {
	/* draw dots */
	gl_fillbox(DOCKCLOCK_X+28, DOCKCLOCK_Y+3, 2, 2, COL_CLOCK_C);
	gl_fillbox(DOCKCLOCK_X+30, DOCKCLOCK_Y+7, 2, 2, COL_CLOCK_S);
	gl_fillbox(DOCKCLOCK_X+28, DOCKCLOCK_Y+12, 2, 2, COL_CLOCK_C);
	gl_fillbox(DOCKCLOCK_X+30, DOCKCLOCK_Y+15, 2, 2, COL_CLOCK_S);
    }
}

void clock_set_time (void)
{
    struct tm *tinfo;
    long timer;

    int w = (10+50+100+10), h = (130);
    int x=(WIDTH-w)/2, xb=x+150, y=(HEIGHT-h)/2;
    char buf[w*h*2];
    char text[5];
    int h_, m_, s_, _m, _d, _y;

    timer = time(NULL);
    tinfo = localtime(&timer);

    s_ = tinfo->tm_sec;
    m_ = tinfo->tm_min;
    h_ = tinfo->tm_hour;
    _d = tinfo->tm_mday;
    _m = tinfo->tm_mon;
    _y = tinfo->tm_year;

    // print screen
    draw_box0(x-10, y-20, w, h, CLOCK_CAPTION, buf);
    // hour minute second
    _drawtext(x+10, y+2, 0, helvR08, HHMMSS, 200, 14, FONTFORCEWIDTH, 1);
    // hour
    draw_button_area3(x+5, y+20, 20, 14);
    sprintf(text, "%02d", tinfo->tm_hour);
    _drawtext(x+10, y+21, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
    // hour -
    _drawtext(x-5, y+21, 0, helvR08, "-", 20, 14, FONTFORCEWIDTH, 1);
    // hour +
    _drawtext(x+30, y+21, 0, helvR08, "+", 20, 14, FONTFORCEWIDTH, 1);
    // minute
    draw_button_area3(x+50, y+20, 20, 14);
    sprintf(text, "%02d", tinfo->tm_min);
    _drawtext(x+55, y+21, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
    // minute -
    _drawtext(x+40, y+21, 0, helvR08, "-", 20, 14, FONTFORCEWIDTH, 1);
    // minute +
    _drawtext(x+75, y+21, 0, helvR08, "+", 20, 14, FONTFORCEWIDTH, 1);
    // second
    draw_button_area3(x+95, y+20, 20, 14);
    sprintf(text, "%02d", tinfo->tm_sec);
    _drawtext(x+100, y+21, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
    // second -
    _drawtext(x+85, y+21, 0, helvR08, "-", 20, 14, FONTFORCEWIDTH, 1);
    // second +
    _drawtext(x+120, y+21, 0, helvR08, "+", 20, 14, FONTFORCEWIDTH, 1);

    _drawtext(x+10, y+42, 0, helvR08, MMDDYYYY, 200, 14, FONTFORCEWIDTH, 1);
    // month
    draw_button_area3(x+5, y+60, 20, 14);
    sprintf(text, "%02d", tinfo->tm_mon);
    _drawtext(x+10, y+61, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
    // month -
    _drawtext(x-5, y+61, 0, helvR08, "-", 20, 14, FONTFORCEWIDTH, 1);
    // month +
    _drawtext(x+30, y+61, 0, helvR08, "+", 20, 14, FONTFORCEWIDTH, 1);
    // day
    draw_button_area3(x+50, y+60, 20, 14);
    sprintf(text, "%02d", tinfo->tm_mday);
    _drawtext(x+55, y+61, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
    // day -
    _drawtext(x+40, y+61, 0, helvR08, "-", 20, 14, FONTFORCEWIDTH, 1);
    // day +
    _drawtext(x+75, y+61, 0, helvR08, "+", 20, 14, FONTFORCEWIDTH, 1);
    // year
    draw_button_area3(x+95, y+60, 40, 14);
    sprintf(text, "%04d", tinfo->tm_year + 1900);
    _drawtext(x+103, y+61, 0, helvR08, text, 40, 14, FONTFORCEWIDTH, 1);
    // year -
    _drawtext(x+85, y+61, 0, helvR08, "-", 20, 14, FONTFORCEWIDTH, 1);
    // year +
    _drawtext(x+140, y+61, 0, helvR08, "+", 20, 14, FONTFORCEWIDTH, 1);
    
    draw_button(x-15+(w-50-50-10)/2, y+85, TEXT_OK, 0);
    draw_button(x-15+60+(w-50-50-10)/2, y+85, TEXT_CANCEL, 0);
    
    while (1) {
	UPDATERS(1);
	if_mouse_pressed_over(x-15+(w-50-50-10)/2, y+85, 50, 19, MOUSE_LEFTBUTTON) {
	    draw_button(x-15+(w-50-50-10)/2, y+85, TEXT_OK, 1);
	    // set variables
	    tinfo->tm_sec = s_;
	    tinfo->tm_min = m_;
	    tinfo->tm_hour = h_;
	    tinfo->tm_mday = _d;
	    tinfo->tm_mon = _m;
	    tinfo->tm_year = _y;
	    //tinfo->tm_isdst =
	    timer = mktime(tinfo);
	    stime(&timer);
	    goto QUIT_CLOCK_SET;
	}
	if_mouse_pressed_over(x-15+60+(w-50-50-10)/2, y+85, 50, 19, MOUSE_LEFTBUTTON) {
	    draw_button(x-15+60+(w-50-50-10)/2, y+85, TEXT_CANCEL, 1);
	    goto QUIT_CLOCK_SET;
	}
	// hour
	if_mouse_pressed_over(x-5, y+21, 5, 14, MOUSE_LEFTBUTTON) {
	    if (h_>0) {
	        h_--;
		gl_fillbox(x+5, y+20, 20, 14, COL_BUTTON_3);
		draw_button_area3(x+5, y+20, 20, 14);
		sprintf(text, "%02d", h_);
		_drawtext(x+10, y+21, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
		goto MOUSE_PRESSED;
	    }
	}
	if_mouse_pressed_over(x+30, y+21, 5, 14, MOUSE_LEFTBUTTON) {
	    if (h_<23) {
	        h_++;
		gl_fillbox(x+5, y+20, 20, 14, COL_BUTTON_3);
		draw_button_area3(x+5, y+20, 20, 14);
		sprintf(text, "%02d", h_);
		_drawtext(x+10, y+21, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
		goto MOUSE_PRESSED;
	    }
	}
	// minute
	if_mouse_pressed_over(x+40, y+21, 5, 14, MOUSE_LEFTBUTTON) {
	    if (m_>0) {
	        m_--;
		gl_fillbox(x+50, y+20, 20, 14, COL_BUTTON_3);
		draw_button_area3(x+50, y+20, 20, 14);
		sprintf(text, "%02d", m_);
		_drawtext(x+55, y+21, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
		goto MOUSE_PRESSED;
	    }
	}
	if_mouse_pressed_over(x+75, y+21, 5, 14, MOUSE_LEFTBUTTON) {
	    if (m_<59) {
	        m_++;
		gl_fillbox(x+50, y+20, 20, 14, COL_BUTTON_3);
		draw_button_area3(x+50, y+20, 20, 14);
		sprintf(text, "%02d", m_);
		_drawtext(x+55, y+21, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
		goto MOUSE_PRESSED;
	    }
	}
	// second
	if_mouse_pressed_over(x+85, y+21, 5, 14, MOUSE_LEFTBUTTON) {
	    if (s_>0) {
	        s_--;
		gl_fillbox(x+95, y+20, 20, 14, COL_BUTTON_3);
		draw_button_area3(x+95, y+20, 20, 14);
		sprintf(text, "%02d", s_);
		_drawtext(x+100, y+21, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
		goto MOUSE_PRESSED;
	    }
	}
	if_mouse_pressed_over(x+120, y+21, 5, 14, MOUSE_LEFTBUTTON) {
	    if (s_<59) {
	        s_++;
		gl_fillbox(x+95, y+20, 20, 14, COL_BUTTON_3);
		draw_button_area3(x+95, y+20, 20, 14);
		sprintf(text, "%02d", s_);
		_drawtext(x+100, y+21, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
		goto MOUSE_PRESSED;
	    }
	}
	// month
	if_mouse_pressed_over(x-5, y+61, 5, 14, MOUSE_LEFTBUTTON) {
	    if (_m>0) {
	        _m--;
		gl_fillbox(x+5, y+60, 20, 14, COL_BUTTON_3);
		draw_button_area3(x+5, y+60, 20, 14);
		sprintf(text, "%02d", _m);
		_drawtext(x+10, y+61, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
		goto MOUSE_PRESSED;
	    }
	}
	if_mouse_pressed_over(x+30, y+61, 5, 14, MOUSE_LEFTBUTTON) {
	    if (_m<11) {
	        _m++;
		gl_fillbox(x+5, y+60, 20, 14, COL_BUTTON_3);
		draw_button_area3(x+5, y+60, 20, 14);
		sprintf(text, "%02d", _m);
		_drawtext(x+10, y+61, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
		goto MOUSE_PRESSED;
	    }
	}
	// day
	if_mouse_pressed_over(x+40, y+61, 5, 14, MOUSE_LEFTBUTTON) {
	    if (_d>0) {
	        _d--;
		gl_fillbox(x+50, y+60, 20, 14, COL_BUTTON_3);
		draw_button_area3(x+50, y+60, 20, 14);
		sprintf(text, "%02d", _d);
		_drawtext(x+55, y+61, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
		goto MOUSE_PRESSED;
	    }
	}
	if_mouse_pressed_over(x+75, y+61, 5, 14, MOUSE_LEFTBUTTON) {
	    if (_d<30) {
	        _d++;
		gl_fillbox(x+50, y+60, 20, 14, COL_BUTTON_3);
		draw_button_area3(x+50, y+60, 20, 14);
		sprintf(text, "%02d", _d);
		_drawtext(x+55, y+61, 0, helvR08, text, 20, 14, FONTFORCEWIDTH, 1);
		goto MOUSE_PRESSED;
	    }
	}
	// year
	if_mouse_pressed_over(x+85, y+61, 5, 14, MOUSE_LEFTBUTTON) {
	    if (_y+1900>1900) {
	        _y--;
		gl_fillbox(x+95, y+60, 40, 14, COL_BUTTON_3);
		draw_button_area3(x+95, y+60, 40, 14);
		sprintf(text, "%02d", _y+1900);
		_drawtext(x+103, y+61, 0, helvR08, text, 40, 14, FONTFORCEWIDTH, 1);
		goto MOUSE_PRESSED;
	    }
	}
	if_mouse_pressed_over(x+140, y+61, 5, 14, MOUSE_LEFTBUTTON) {
	    if (_y+1900<3000) {
	        _y++;
		gl_fillbox(x+95, y+60, 40, 14, COL_BUTTON_3);
		draw_button_area3(x+95, y+60, 40, 14);
		sprintf(text, "%02d", _y+1900);
		_drawtext(x+103, y+61, 0, helvR08, text, 40, 14, FONTFORCEWIDTH, 1);
		goto MOUSE_PRESSED;
	    }
	}
MOUSE_PRESSED:
	_mouse_waitbuttonrealese();
    }
QUIT_CLOCK_SET:
    gl_putbox(x-10, y-20, w, h, buf);
    return;
}
