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
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <vga.h>
#include <vgagl.h>
#include <vgamouse.h>

#include "main.h"
#include "font.h"
#include "utils.h"
#include "mouse.h"

#ifdef DEBUG
void panic (char *filename, int line)
{
    (void)fprintf(stderr,"\n?Panic in line %d of file %s\n", line, filename);
    (void)perror("Unexpected library error");
    abort();
}

void TEllME(char *filename, int line)
{
    (void)fprintf(stderr,"\n?I am in line %d of file %s\n", line, filename);
}
#endif

int sleep_nano (long nsec)
{
    struct timespec req;
    struct timespec rem;

    req.tv_sec = 0;
    req.tv_nsec = nsec;
    nanosleep(&req, &rem);
}

int _rand (int max)
{
    return (1+(int) (((float) max)*rand()/(RAND_MAX+1.0)));
}
    

void sigset (int signum, void(func)(int foo))
{
    struct sigaction sa;
    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(signum, &sa, NULL);
}

void draw_button_area0 (int x, int y, int w, int h)
{
    gl_fillbox(x, y, w, h, COL_BUTTON_4); /* 4 */
    gl_fillbox(x, y, w-1, h-1, COL_BUTTON_0); /* 0 */
    gl_fillbox(x+1, y+1, w-2, h-2, COL_BUTTON_1); /* 1 */
    gl_fillbox(x+1, y+1, w-3, h-3, COL_BUTTON_2); /* 2 */
    gl_fillbox(x+2, y+2, w-4, h-4, COL_BUTTON_3); /* 3 */
}

void draw_tetro_area0 (int x, int y, int w, int h)
{
    gl_fillbox(x, y, w, h, COL_BUTTON_4); /* 4 */
    gl_fillbox(x, y, w-1, h-1, COL_BUTTON_0); /* 0 */
    gl_fillbox(x+1, y+1, w-2, h-2, COL_BUTTON_1); /* 1 */
    gl_fillbox(x+1, y+1, w-3, h-3, COL_BUTTON_2); /* 2 */
    gl_fillbox(x+2, y+2, w-4, h-4, gl_rgbcolor(0, 0, 255)); /* 3 */
}

void draw_button_area1 (int x, int y, int w, int h)
{
    gl_fillbox(x, y, w, h, COL_BUTTON_0); /* 0 */
    gl_fillbox(x, y, w-1, h-1, COL_BUTTON_4); /* 4 */
    gl_fillbox(x+1, y+1, w-2, h-2, COL_BUTTON_2); /* 2 */
    gl_fillbox(x+1, y+1, w-3, h-3, COL_BUTTON_1); /* 1 */
    gl_fillbox(x+2, y+2, w-4, h-4, COL_BUTTON_3); /* 3 */
}

void draw_button_area2 (int x, int y, int w, int h)
{
    gl_line(x, y, x+w, y, COL_BUTTON_0);
    gl_line(x, y, x, y+h, COL_BUTTON_0);
    gl_line(x, y+h, x+w, y+h, COL_BUTTON_4);
    gl_line(x+w, y, x+w, y+h, COL_BUTTON_4);
}

void draw_button_area3 (int x, int y, int w, int h)
{
    gl_line(x, y, x+w, y, COL_BUTTON_4);
    gl_line(x, y, x, y+h, COL_BUTTON_4);
    gl_line(x, y+h, x+w, y+h, COL_BUTTON_0);
    gl_line(x+w, y, x+w, y+h, COL_BUTTON_0);
}
void draw_button_area31 (int x, int y, int w, int h)
{
    gl_line(x, y, x+w, y, COL_BUTTON_4);
    gl_line(x+1, y+1, x+w-2, y+1, COL_BUTTON_1);
    gl_line(x, y, x, y+h, COL_BUTTON_4);
    gl_line(x+1, y+1, x+1, y+h-2, COL_BUTTON_1);
    gl_line(x, y+h, x+w, y+h, COL_BUTTON_0);
    gl_line(x+1, y+h-1, x+w-1, y+h-1, COL_BUTTON_2);
    gl_line(x+w, y, x+w, y+h, COL_BUTTON_0);
    gl_line(x+w-1, y+1, x+w-1, y+h-2, COL_BUTTON_2);
}

void draw_box0 (int x, int y, int w, int h, char *toptext, char *buf)
{
    int i=0;
    // dir browser area
    gl_getbox(x, y, w, h, buf);
    draw_button_area0(x, y, w, h);
    // top line
    gl_line(x, y+14, x+w-3, y+14, COL_BUTTON_4);
    gl_line(x, y+15, x+w-2, y+15, COL_BUTTON_0);
    // drawtext
    i = _drawtext(0, 0, 0, helvR08, toptext, 0, 13, FONTSCROLL, 0);
    _drawtext(x+((w-i)/2), y+2, COL_BUTTON_TEXT, helvR08, toptext, ((i < w) ? i : w), 13, FONTNOSCROLL, 1);
}

void info_box (char *tstr, char *str)
{
    int i, j, x, y, w, h;
    char *buf;
    
    i = _drawtext(0, 0, 0, helvR08, str, 0, 13, FONTSCROLL, 0);
    j = _drawtext(0, 0, 0, helvR08, tstr, 0, 13, FONTSCROLL, 0);
    w = ((i > j) ? i : j) + 15;
    h = 15 + 10 + 13 + 21;
    x = ((WIDTH - w) / 2);
    y = ((HEIGHT - h) / 2);
    buf = (char *) calloc(2, w*h);
    draw_box0(x, y, w, h, tstr, buf);
    _drawtext(x+((w-i)/2), y+20, COL_BUTTON_TEXT, helvR08, str, ((i < w) ? i : w), 13, FONTNOSCROLL, 1);
    draw_button(x+((w-50)/2), y+20+15, TEXT_OK, 0);
    while (1) {
	UPDATERS(1);
	if_mouse_pressed_over(x+((w-50)/2), y+20+15, 50, 19, MOUSE_LEFTBUTTON) {
	    draw_button(x+((w-50)/2), y+20+15, TEXT_OK, 1);
	    break;
	}
    }
    gl_putbox(x, y, w, h, buf);
    free(buf);
}

int question_box (char *tstr, char *str)
{
    int i, j, x, y, w, h, ret;
    char *buf;
    
    i = _drawtext(0, 0, 0, helvR08, str, 0, 13, FONTSCROLL, 0);
    j = _drawtext(0, 0, 0, helvR08, tstr, 0, 13, FONTSCROLL, 0);
    w = ((i > j) ? i : j) + 15;
    h = 15 + 10 + 13 + 21;
    x = ((WIDTH - w) / 2);
    y = ((HEIGHT - h) / 2);
    buf = (char *) calloc(2, w*h);
    draw_box0(x, y, w, h, tstr, buf);
    _drawtext(x+((w-i)/2), y+20, COL_BUTTON_TEXT, helvR08, str, ((i < w) ? i : w), 13, FONTNOSCROLL, 1);
    draw_button(x+((w-110)/2), y+20+15, TEXT_OK, 0);
    draw_button(x+((w-110)/2)+60, y+20+15, TEXT_CANCEL, 0);
    while (1) {
	UPDATERS(1);
	if_mouse_pressed_over(x+((w-110)/2), y+20+15, 50, 19, MOUSE_LEFTBUTTON) {
	    draw_button(x+((w-110)/2), y+20+15, TEXT_OK, 1);
	    ret = 1; break;
	}
	if_mouse_pressed_over(x+((w-110)/2)+60, y+20+15, 50, 19, MOUSE_LEFTBUTTON) {
	    draw_button(x+((w-110)/2)+60, y+20+15, TEXT_CANCEL, 1);
	    ret = 0; break;
	}
    }
    gl_putbox(x, y, w, h, buf);
    free(buf);
    return ret;
}

void draw_button (int x, int y, char *str, int p)
{
    int i;
    i = _drawtext(0, 0, 0, helvR08, str, 0, 13, FONTSCROLL, 0);

    if (p)
        draw_button_area1(x, y, 50, 19);
    else
	draw_button_area0(x, y, 50, 19);

    _drawtext(x+((50-i)/2), y+4, COL_BUTTON_TEXT, helvR08, str, 50, 13, FONTNOSCROLL, 1);

    if (p)
	_mouse_waitbuttonrealese();
}

int draw_popup (int x, int y, char *tstr, struct popup *tmp)
{
    int l = 0, i, j;
    int w, h;
    char *buf;
    
    for (i = 0; i < tmp->i; i++) {
	j = _drawtext(0, 0, 0, helvR08, tmp->item[i], 0, 13, FONTSCROLL, 0);
	l = (l > j) ? l : j;
    }
    j = _drawtext(0, 0, 0, helvR08, tstr, 0, 13, FONTSCROLL, 0);
    l = (l > j) ? l : j;
    
    w = l + 15;
    h = 15 + 4 + 13 * (tmp->i);
    buf = (char *) calloc(2, w*h);
    draw_box0(x, y, w, h, tstr, buf);
    for (i = 0; i < tmp->i; i++)
	_drawtext(x+7, y+17+(13*i), COL_BUTTON_TEXT, helvR08, tmp->item[i], 0, 13, FONTSCROLL, 1);
    _mouse_waitbuttonrealese();
    while (1) {
	UPDATERS(1);
	for (i = -1; i < tmp->i; i++)
	    if_mouse_pressed_over(x+7, y+17+(13*i), w, 13, MOUSE_LEFTBUTTON) {
		gl_fillbox(x+3, y+17+(13*i)+3, 3, 5, COL_BUTTON_TEXT);
		_mouse_waitbuttonrealese();
		gl_putbox(x, y, w, h, buf);
		free(buf);
		return i;
	    }
	    if_mouse_pressed_over(0, 0, 640, 480, MOUSE_LEFTBUTTON) {
		gl_putbox(x, y, w, h, buf);
		_mouse_waitbuttonrealese();
		free(buf);
		return -1;
	    }
	    if_mouse_pressed_over(0, 0, 640, 480, MOUSE_RIGHTBUTTON) {
		gl_putbox(x, y, w, h, buf);
		_mouse_waitbuttonrealese();
		free(buf);
		return -1;
	    }
    }
}

void draw_slider (int x, int y, int h, int parts, int part)
{
    // up angle
    _gl_triangle(x+5, y, COL_BUTTON_4, x+10, y+20, COL_BUTTON_4, x, y+20, COL_BUTTON_0);
    // down angle
    _gl_triangle(x+5, y+h, COL_BUTTON_4, x+10, y+h-20, COL_BUTTON_4, x, y+h-20, COL_BUTTON_0);
    // slider box
    draw_button_area0(x+1, y+20+5+((h-40-8)/parts)*(part-1), 9, (h-40-8)/parts);
}

void _gl_triangle (int x0, int y0, int c0, int x1, int y1, int c1, int x2, int y2, int c2)
{
    gl_line(x0, y0, x1, y1, c0);
    gl_line(x1, y1, x2, y2, c1);
    gl_line(x2, y2, x0, y0, c2);
}

static int ppm_i = 0;
void write_ppm (void)
{
    int x, y, r, g, b, i=0;
    char img_name[255];
    int fd;
    char buf[640*480*13];
    
    sprintf(img_name, "/home/out_%d.ppm", ppm_i);
    
    fd = open(img_name, O_RDWR | O_CREAT, 00644);
    ppm_i++;
    
    sprintf(buf, "P3\n"
		 "640 480\n"
		 "255\n");
    write(fd, buf, strlen(buf));
    
    for (y=0; y<480; y++)
	for (x=0; x<640; x++) {
	    if ((i%36) == 0) {
		write(fd, "\n", 1);
		i = 0;
	    }
	    gl_getpixelrgb(x, y, &r, &g, &b);
	    sprintf(buf, "%d %d %d ", r, g, b);
	    write(fd, buf, strlen(buf));
	    i++;
	}
    close(fd);
}
