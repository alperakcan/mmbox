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
#include "../../main.h"
#include "../../mouse.h"

#define W 10
#define H 20
#define GAME_AREA_W (W*20+15)
#define GAME_AREA_H (H*20+15)
#define INST_AREA_W (220)
#define INST_AREA_H (220)
#define GAME_AREA_X (((640-GAME_AREA_W-INST_AREA_W)/2)-20)
#define GAME_AREA_Y (480-GAME_AREA_H)/2
#define INST_AREA_X (GAME_AREA_X+GAME_AREA_W+10)
#define INST_AREA_Y GAME_AREA_Y

static int screen_matrix[H][W];
static int xold;
static int yold;
static int mxold = 0;
static int myold = 0;
static int score = 0;
static int level = 1;

// tetro 62 looks like our logo, so it must have extra point ;)
static int tetro_tetromino = 0;

void draw_tetro_xx (int x0, int y0,
		    int x1, int y1,
		    int x2, int y2,
		    int x3, int y3,
		    int my0, int mx0,
		    int my1, int mx1,
		    int my2, int mx2,
		    int my3, int mx3)
{
    if (tetro_tetromino) {
        draw_tetro_area0(x0, y0, 20, 20);
	draw_tetro_area0(x1, y1, 20, 20);
	draw_tetro_area0(x2, y2, 20, 20);
	draw_tetro_area0(x3, y3, 20, 20);
    } else {	
	draw_button_area0(x0, y0, 20, 20);
	draw_button_area0(x1, y1, 20, 20);
	draw_button_area0(x2, y2, 20, 20);
	draw_button_area0(x3, y3, 20, 20);
    }
    screen_matrix[my0][mx0] = screen_matrix[my1][mx1] = screen_matrix[my2][mx2] = screen_matrix[my3][mx3] = 1;
}
void clean_tetro_xx (int x0, int y0,
		     int x1, int y1,
		     int x2, int y2,
		     int x3, int y3,
		     int my0, int mx0,
		     int my1, int mx1,
		     int my2, int mx2,
		     int my3, int mx3)
{
    if (y0>GAME_AREA_Y) {
	gl_fillbox(x0, y0, 20, 20, 0);
	gl_fillbox(x1, y1, 20, 20, 0);
	gl_fillbox(x2, y2, 20, 20, 0);
	gl_fillbox(x3, y3, 20, 20, 0);
    }
    screen_matrix[my0][mx0] = screen_matrix[my1][mx1] = screen_matrix[my2][mx2] = screen_matrix[my3][mx3] = 0;
    mxold = -1;
    myold = -1;
}
int can_tetro_xx (int x, int y, int m0, int m1, int m2, int m3)
{
    if ((x>=W) || (y>=H))
	return 0;
    if ((m0 == 0) && (m1 == 0) && (m2 == 0) && (m3 == 0))
    	return 1;
    else
	return 0;
}

void tetro_x_xx (int *mx, int m, int *x)
{
    if ((*mx)+m>=W) {
	(*x)-=(20*((*mx)+m+1-W));
	(*mx)-=((*mx)+m+1-W);
    }
    while ((*mx)<0) {
	(*mx)++;
	(*x)+=20;
    }
}
int tetro_xx (int *x, int *y,
	      int mx, int my, int m_y,
	      int m00, int m01, int m02, int m03,
	      int m10, int m11, int m12, int m13,
	      void (*clean) (int, int, int, int),
	      void (*draw) (int, int, int, int),
	      int (*last_check) (int, int))
{
    if (my+m_y>=H) {
	mxold = -1;
	return 0;
    }
    if (mxold<mx) {
        if ((m00 == 1) || (m01 == 1) || (m02 == 1) || (m03 == 1)) {
    	    mx = mxold;
	    (*x) = xold;
	}
    }
    if (mxold>mx) {
	if ((m10 == 1) || (m11 == 1) || (m12 == 1) || (m13 == 1)) {
	    mx = mxold;
	    (*x) = xold;
	}
    }
    if (mxold==mx) {
	if (last_check(mx, my) == 0) {
	    mxold = -1;
	    return 0;
	}
    }
    if (mxold>=0)
        clean(xold, yold, mxold, myold);
    draw((*x), (*y), mx, my);
    xold = (*x);
    yold = (*y);
    mxold = mx;
    myold = my;
    return 1;
}

/* **** */
void draw_tetro_00 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x, y, x+20, y, x+40, y, x+60, y,
		  my, mx, my, mx+1, my, mx+2, my, mx+3);
}
void clean_tetro_00 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x, y, x+20, y, x+40, y, x+60, y,
		  my, mx, my, mx+1, my, mx+2, my, mx+3);
}
int can_tetro_00 (int mx, int my)
{
    return (can_tetro_xx(mx+3, my+1, screen_matrix[my][mx+1],
				       screen_matrix[my][mx+2],
				       screen_matrix[my][mx+3],
				       0));
}
int check_tetro_00 (int mx, int my)
{
    if ((screen_matrix[my][mx] == 1) ||
	(screen_matrix[my][mx+1] == 1) ||
	(screen_matrix[my][mx+2] == 1) ||
	(screen_matrix[my][mx+3] == 1))
	return 0;
    else
	return 1;
}
int tetro_00 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 3, x);
    return (tetro_xx(x, y, mx, my, 0,
	      screen_matrix[my][mx], screen_matrix[my][mx+1], screen_matrix[my][mx+2], screen_matrix[my][mx+3],
	      screen_matrix[my][mx], screen_matrix[my][mx+1], screen_matrix[my][mx+2], screen_matrix[my][mx+3],
	      clean_tetro_00, draw_tetro_00, check_tetro_00));
}

/* *
   *
   *
   *
 */
void draw_tetro_01 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x, y, x, y+20, x, y+40, x, y+60,
		  my, mx, my+1, mx, my+2, mx, my+3, mx);
}
void clean_tetro_01 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x, y, x, y+20, x, y+40, x, y+60,
		  my, mx, my+1, mx, my+2, mx, my+3, mx);
}
int can_tetro_01 (int mx, int my)
{
    return (can_tetro_xx(mx, my+4, screen_matrix[my][mx],
				     screen_matrix[my+1][mx],
				     screen_matrix[my+2][mx],
				     screen_matrix[my+3][mx]));
}
int check_tetro_01 (int mx, int my)
{
    if (screen_matrix[my+3][mx] == 1)
	return 0;
    else
	return 1;
}
int tetro_01 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 0, x);
    return (tetro_xx(x, y, mx, my, 3,
		    screen_matrix[my][mx], screen_matrix[my+1][mx], screen_matrix[my+2][mx], screen_matrix[my+3][mx],
		    screen_matrix[my][mx], screen_matrix[my+1][mx], screen_matrix[my+2][mx], screen_matrix[my+3][mx],
	            clean_tetro_01, draw_tetro_01, check_tetro_01));
}

/* **
   **
 */
void draw_tetro_10 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x, y, x, y+20, x+20, y, x+20, y+20,
		  my, mx, my, mx+1, my+1, mx, my+1, mx+1);
}
void clean_tetro_10 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x, y, x, y+20, x+20, y, x+20, y+20,
		  my, mx, my, mx+1, my+1, mx, my+1, mx+1);
}
int check_tetro_10 (int mx, int my)
{
    if ((screen_matrix[my+1][mx] == 1) ||
	(screen_matrix[my+1][mx+1] == 1))
	    return 0;
    else
	return 1;
}
int tetro_10 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 1, x);
    return (tetro_xx(x, y, mx, my, 1,
		    screen_matrix[my][mx+1], screen_matrix[my+1][mx], screen_matrix[my+1][mx+1], 0,
		    screen_matrix[my][mx], screen_matrix[my+1][mx], screen_matrix[my+1][mx+1], 0,
		    clean_tetro_10, draw_tetro_10, check_tetro_10));
}

/*  **
   **
 */
void draw_tetro_20 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x+20, y, x+40, y, x, y+20, x+20, y+20,
		  my, mx+1, my, mx+2, my+1, mx, my+1, mx+1);
}
void clean_tetro_20 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x+20, y, x+40, y, x, y+20, x+20, y+20,
		  my, mx+1, my, mx+2, my+1, mx, my+1, mx+1);
}
int can_tetro_20 (int mx, int my)
{
    return (can_tetro_xx(mx+2, my+2, screen_matrix[my][mx+2],
				       screen_matrix[my+1][mx],
				       0,
				       0));
}
int check_tetro_20 (int mx, int my)
{
    if ((screen_matrix[my][mx+2] == 1) ||
	(screen_matrix[my+1][mx] == 1) ||
	(screen_matrix[my+1][mx+1] == 1))
	    return 0;
    else
	return 1;
}
int tetro_20 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 2, x);
    return (tetro_xx(x, y, mx, my, 1,
		    screen_matrix[my][mx+1], screen_matrix[my][mx+2], screen_matrix[my+1][mx], screen_matrix[my+1][mx+1],
		    screen_matrix[my+1][mx], screen_matrix[my+1][mx+1], 0, 0,
		    clean_tetro_20, draw_tetro_20, check_tetro_20));
}

/* *
   **
    *
 */
void draw_tetro_21 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x, y, x, y+20, x+20, y+20, x+20, y+40,
		  my, mx, my+1, mx, my+1, mx+1, my+2, mx+1);
}
void clean_tetro_21 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x, y, x, y+20, x+20, y+20, x+20, y+40,
		  my, mx, my+1, mx, my+1, mx+1, my+2, mx+1);
}
int can_tetro_21 (int mx, int my)
{
    return (can_tetro_xx(mx+1, my+3, screen_matrix[my+1][mx],
				       screen_matrix[my+1][mx+1],
				       screen_matrix[my+2][mx+1],
				       0));
}
int check_tetro_21 (int mx, int my)
{
    if ((screen_matrix[my+1][mx] == 1) ||
	(screen_matrix[my+2][mx+1] == 1))
	    return 0;
    else
	return 1;
}
int tetro_21 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 1, x);
    return (tetro_xx(x, y, mx, my, 2,
	    screen_matrix[my+1][mx+1], screen_matrix[my+2][mx+1], 0, 0,
	    screen_matrix[my][mx], screen_matrix[my+1][mx], screen_matrix[my+1][mx+1], screen_matrix[my+2][mx+1],
    	    clean_tetro_21, draw_tetro_21, check_tetro_21));
}

/* **
    **
 */
void draw_tetro_30 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x, y, x+20, y, x+20, y+20, x+40, y+20,
		  my, mx, my, mx+1, my+1, mx+1, my+1, mx+2);
}
void clean_tetro_30 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x, y, x+20, y, x+20, y+20, x+40, y+20,
		  my, mx, my, mx+1, my+1, mx+1, my+1, mx+2);
}
int can_tetro_30 (int mx, int my)
{
    return (can_tetro_xx(mx+2, my+2, screen_matrix[my+1][mx+1],
				       screen_matrix[my+1][mx+2],
				       0,
				       0));
}
int check_tetro_30 (int mx, int my)
{
    if ((screen_matrix[my][mx] == 1) ||
	(screen_matrix[my+1][mx+1] == 1) ||
	(screen_matrix[my+1][mx+2] == 1))
	    return 0;
    else
	return 1;
}
int tetro_30 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 2, x);
    return (tetro_xx(x, y, mx, my, 1,
	    screen_matrix[my+1][mx+1], screen_matrix[my+1][mx+2], 0 ,0,
	    screen_matrix[my][mx], screen_matrix[my][mx+1], screen_matrix[my+1][mx+1], screen_matrix[my+1][mx+2],
	    clean_tetro_30, draw_tetro_30, check_tetro_30));
}

/*  *
   **
   *
 */
void draw_tetro_31 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x+20, y, x, y+20, x+20, y+20, x, y+40,
		  my, mx+1, my+1, mx, my+1, mx+1, my+2, mx);
}
void clean_tetro_31 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x+20, y, x, y+20, x+20, y+20, x, y+40,
		  my, mx+1, my+1, mx, my+1, mx+1, my+2, mx);
}
int can_tetro_31 (int mx, int my)
{
    return (can_tetro_xx(mx+1, my+3, screen_matrix[my+1][mx],
				       screen_matrix[my+1][mx+1],
				       screen_matrix[my+2][mx],
				       0));
}
int check_tetro_31 (int mx, int my)
{
    if ((screen_matrix[my+1][mx+1] == 1) ||
	(screen_matrix[my+2][mx] == 1))
	    return 0;
    else
	return 1;
}
int tetro_31 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 1, x);
    return (tetro_xx(x, y, mx, my, 2,
	    screen_matrix[my][mx+1], screen_matrix[my+1][mx], screen_matrix[my+1][mx+1], screen_matrix[my+2][mx],
	    screen_matrix[my+1][mx], screen_matrix[my+2][mx], 0, 0,
	    clean_tetro_31, draw_tetro_31, check_tetro_31));
}

/* ***
     *
 */
void draw_tetro_40 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x, y, x+20, y, x+40, y, x+40, y+20,
		  my, mx, my, mx+1, my, mx+2, my+1, mx+2);
}
void clean_tetro_40 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x, y, x+20, y, x+40, y, x+40, y+20,
		  my, mx, my, mx+1, my, mx+2, my+1, mx+2);
}
int can_tetro_40 (int mx, int my)
{
    return (can_tetro_xx(mx+2, my+2, screen_matrix[my][mx],
				       screen_matrix[my][mx+2],
				       0,
				       0));
}
int check_tetro_40 (int mx, int my)
{
    if ((screen_matrix[my][mx] == 1) ||
	(screen_matrix[my][mx+1] == 1) ||
	(screen_matrix[my+1][mx+2] == 1))
	    return 0;
    else
	return 1;
}
int tetro_40 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 2, x);
    return (tetro_xx(x, y, mx, my, 1,
	    screen_matrix[my][mx], screen_matrix[my][mx+2], screen_matrix[my+1][mx+2], 0,
	    screen_matrix[my][mx], screen_matrix[my][mx+1], screen_matrix[my][mx+2], screen_matrix[my+1][mx+2],
            clean_tetro_40, draw_tetro_40, check_tetro_40));
}

/* **
   *
   *
 */
void draw_tetro_41 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x, y, x+20, y, x, y+20, x, y+40,
		  my, mx, my, mx+1, my+1, mx, my+2, mx);
}
void clean_tetro_41 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x, y, x+20, y, x, y+20, x, y+40,
		  my, mx, my, mx+1, my+1, mx, my+2, mx);
}
int can_tetro_41 (int mx, int my)
{
    return (can_tetro_xx(mx+1, my+3, screen_matrix[my][mx],
				       screen_matrix[my][mx+1],
				       screen_matrix[my+1][mx],
				       screen_matrix[my+2][mx]));
}
int check_tetro_41 (int mx, int my)
{
    if ((screen_matrix[my][mx+1] == 1) ||
	(screen_matrix[my+2][mx] == 1))
	    return 0;
    else
	return 1;
}
int tetro_41 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 1, x);
    return (tetro_xx(x, y, mx, my, 2,
	    screen_matrix[my][mx], screen_matrix[my][mx+1], screen_matrix[my+1][mx], screen_matrix[my+2][mx],
	    screen_matrix[my][mx], screen_matrix[my+1][mx], screen_matrix[my+2][mx], 0,
	    clean_tetro_41, draw_tetro_41, check_tetro_41));
}

/* *
   ***
 */
void draw_tetro_42 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x, y, x, y+20, x+20, y+20, x+40, y+20,
		  my, mx, my+1, mx, my+1, mx+1, my+1, mx+2);
}
void clean_tetro_42 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x, y, x, y+20, x+20, y+20, x+40, y+20,
		  my, mx, my+1, mx, my+1, mx+1, my+1, mx+2);
}
int can_tetro_42 (int mx, int my)
{
    return (can_tetro_xx(mx+2, my+2, screen_matrix[my+1][mx+1],
				       screen_matrix[my+1][mx+2],
				       0,
				       0));
}
int check_tetro_42 (int mx, int my)
{
    if ((screen_matrix[my+1][mx] == 1) ||
	(screen_matrix[my+1][mx+1] == 1) ||
	(screen_matrix[my+1][mx+2] == 1))
	    return 0;
    else
	return 1;
}
int tetro_42 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 2, x);
    return (tetro_xx(x, y, mx, my, 1,
	    screen_matrix[my+1][mx], screen_matrix[my+1][mx+1], screen_matrix[my+1][mx+2], 0,
	    screen_matrix[my][mx], screen_matrix[my+1][mx], screen_matrix[my+1][mx+1], screen_matrix[my+1][mx+2],
	    clean_tetro_42, draw_tetro_42, check_tetro_42));
}

/*  *
    *
   **
 */
void draw_tetro_43 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x+20, y, x+20, y+20, x, y+40, x+20, y+40,
		  my, mx+1, my+1, mx+1, my+2, mx, my+2, mx+1);
}
void clean_tetro_43 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x+20, y, x+20, y+20, x, y+40, x+20, y+40,
		  my, mx+1, my+1, mx+1, my+2, mx, my+2, mx+1);
}
int can_tetro_43 (int mx, int my)
{
    return (can_tetro_xx(mx+1, my+3, screen_matrix[my+1][mx+1],
				       screen_matrix[my+2][mx],
				       screen_matrix[my+2][mx+1],
				       0));
}
int check_tetro_43 (int mx, int my)
{
    if ((screen_matrix[my+2][mx] == 1) ||
	(screen_matrix[my+2][mx+1] == 1))
	    return 0;
    return 1;
}
int tetro_43 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 1, x);
    return (tetro_xx(x, y, mx, my, 2,
	    screen_matrix[my][mx+1], screen_matrix[my+1][mx+1], screen_matrix[my+2][mx], screen_matrix[my+2][mx+1],
	    screen_matrix[my][mx+1], screen_matrix[my+2][mx], screen_matrix[my+2][mx+1], 0,
	    clean_tetro_43, draw_tetro_43, check_tetro_43));
}

/* ***
   *
 */
void draw_tetro_50 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x, y, x+20, y, x+40, y, x, y+20,
		  my, mx, my, mx+1, my, mx+2, my+1, mx);
}
void clean_tetro_50 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x, y, x+20, y, x+40, y, x, y+20,
		  my, mx, my, mx+1, my, mx+2, my+1, mx);
}
int can_tetro_50 (int mx, int my)
{
    return (can_tetro_xx(mx+2, my+2, screen_matrix[my][mx],
				       screen_matrix[my][mx+2],
				       screen_matrix[my+1][mx],
				       0));
}
int check_tetro_50 (int mx, int my)
{
    if ((screen_matrix[my][mx+1] == 1) ||
	(screen_matrix[my][mx+2] == 1) ||
	(screen_matrix[my+1][mx] == 1))
	    return 0;
    else
	return 1;
}
int tetro_50 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 2, x);
    return (tetro_xx(x, y, mx, my, 1,
	    screen_matrix[my][mx], screen_matrix[my][mx+1], screen_matrix[my][mx+2], screen_matrix[my+1][mx],
	    screen_matrix[my][mx], screen_matrix[my][mx+2], screen_matrix[my+1][mx], 0,
	    clean_tetro_50, draw_tetro_50, check_tetro_50));
}

/* *
   *
   **
 */
void draw_tetro_51 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x, y, x, y+20, x, y+40, x+20, y+40,
		  my, mx, my+1, mx, my+2, mx, my+2, mx+1);
}
void clean_tetro_51 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x, y, x, y+20, x, y+40, x+20, y+40,
		  my, mx, my+1, mx, my+2, mx, my+2, mx+1);
}
int can_tetro_51 (int mx, int my)
{
    return (can_tetro_xx(mx+1, my+3, screen_matrix[my+1][mx],
				       screen_matrix[my+2][mx],
				       screen_matrix[my+2][mx+1],
				       0));
}
int check_tetro_51 (int mx, int my)
{	
    if ((screen_matrix[my+2][mx] == 1) ||
	(screen_matrix[my+2][mx+1] == 1))
	    return 0;
    else
	return 1;
}
int tetro_51 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 1, x);
    return (tetro_xx(x, y, mx, my, 2,
	    screen_matrix[my][mx], screen_matrix[my+2][mx], screen_matrix[my+2][mx+1], 0,
	    screen_matrix[my][mx], screen_matrix[my+1][mx], screen_matrix[my+2][mx], screen_matrix[my+2][mx+1],
	    clean_tetro_51, draw_tetro_51, check_tetro_51));
}

/*   *
   ***
 */
void draw_tetro_52 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x+40, y, x, y+20, x+20, y+20, x+40, y+20,
		  my, mx+2, my+1, mx, my+1, mx+1, my+1, mx+2);
}
void clean_tetro_52 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x+40, y, x, y+20, x+20, y+20, x+40, y+20,
		  my, mx+2, my+1, mx, my+1, mx+1, my+1, mx+2);
}
int can_tetro_52 (int mx, int my)
{
    return (can_tetro_xx(mx+2, my+2, screen_matrix[my][mx+2],
				       screen_matrix[my+1][mx+2],
				       0,
				       0));
}
int check_tetro_52 (int mx, int my)
{
    if ((screen_matrix[my+1][mx] == 1) ||
	(screen_matrix[my+1][mx+1] == 1) ||
	(screen_matrix[my+1][mx+2] == 1))
	    return 0;
    else
	return 1;
}
int tetro_52 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 2, x);
    return (tetro_xx(x, y, mx, my, 1,
	    screen_matrix[my][mx+2], screen_matrix[my+1][mx], screen_matrix[my+1][mx+1], screen_matrix[my+1][mx+2],
	    screen_matrix[my+1][mx], screen_matrix[my+1][mx+1], screen_matrix[my+1][mx+2], 0,
	    clean_tetro_52, draw_tetro_52, check_tetro_52));
}

/*  **
     *
     *
 */
void draw_tetro_53 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x, y, x+20, y, x+20, y+20, x+20, y+40,
		  my, mx, my, mx+1, my+1, mx+1, my+2, mx+1);
}
void clean_tetro_53 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x, y, x+20, y, x+20, y+20, x+20, y+40,
		  my, mx, my, mx+1, my+1, mx+1, my+2, mx+1);
}
int can_tetro_53 (int mx, int my)
{
    return (can_tetro_xx(mx+1, my+3, screen_matrix[my+1][mx+1],
				       screen_matrix[my+2][mx+1],
				       0,
				       0));
}
int check_tetro_53 (int mx, int my)
{
    if ((screen_matrix[my][mx] == 1) ||
	(screen_matrix[my+2][mx+1] == 1))
	    return 0;
    else
	return 1;
}
int tetro_53 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 1, x);
    return (tetro_xx(x, y, mx, my, 2,
	    screen_matrix[my][mx+1], screen_matrix[my+1][mx+1], screen_matrix[my+2][mx+1], 0,
	    screen_matrix[my][mx], screen_matrix[my][mx+1], screen_matrix[my+1][mx+1], screen_matrix[my+2][mx+1],
            clean_tetro_53, draw_tetro_53, check_tetro_53));
}

/*  *
   ***
 */
void draw_tetro_60 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x+20, y, x, y+20, x+20, y+20, x+40, y+20,
		  my, mx+1, my+1, mx, my+1, mx+1, my+1, mx+2);
}
void clean_tetro_60 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x+20, y, x, y+20, x+20, y+20, x+40, y+20,
		  my, mx+1, my+1, mx, my+1, mx+1, my+1, mx+2);
}
int can_tetro_60 (int mx, int my)
{
    return (can_tetro_xx(mx+2, my+2, screen_matrix[my+1][mx+1],
				       screen_matrix[my+1][mx+2],
				       0,
				       0));
}
int check_tetro_60 (int mx, int my)
{
    if ((screen_matrix[my+1][mx] == 1) ||
	(screen_matrix[my+1][mx+1] == 1) ||
	(screen_matrix[my+1][mx+2] == 1))
	    return 0;
    else
	return 1;
}
int tetro_60 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 2, x);
    return (tetro_xx(x, y, mx, my, 1,
	    screen_matrix[my+1][mx], screen_matrix[my+1][mx+1], screen_matrix[my+1][mx+2], 0,
	    screen_matrix[my+1][mx], screen_matrix[my+1][mx+1], screen_matrix[my+1][mx+2], 0,
	    clean_tetro_60, draw_tetro_60, check_tetro_60));
}

/*  *
   **
    *
 */
void draw_tetro_61 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x+20, y, x, y+20, x+20, y+20, x+20, y+40,
		  my, mx+1, my+1, mx, my+1, mx+1, my+2, mx+1);
}
void clean_tetro_61 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x+20, y, x, y+20, x+20, y+20, x+20, y+40,
		  my, mx+1, my+1, mx, my+1, mx+1, my+2, mx+1);
}
int can_tetro_61 (int mx, int my)
{
    return (can_tetro_xx(mx+1, my+3, screen_matrix[my+1][mx],
				       screen_matrix[my+1][mx+1],
				       screen_matrix[my+2][mx+1],
				       0));
}
int check_tetro_61 (int mx, int my)
{
    if ((screen_matrix[my+1][mx] == 1) ||
	(screen_matrix[my+2][mx+1] == 1))
	    return 0;
    else
	return 1;
}
int tetro_61 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 1, x);
    return (tetro_xx(x, y, mx, my, 2,
	    screen_matrix[my][mx+1], screen_matrix[my+1][mx+1], screen_matrix[my+2][mx+1], 0,
	    screen_matrix[my+1][mx], screen_matrix[my+1][mx+1], screen_matrix[my+2][mx+1], 0,
	    clean_tetro_61, draw_tetro_61, check_tetro_61));
}

/* ***
    *
 */
void draw_tetro_62 (int x, int y, int mx, int my)
{
    tetro_tetromino = 1;
    draw_tetro_xx(x, y, x+20, y, x+40, y, x+20, y+20,
		  my, mx, my, mx+1, my, mx+2, my+1, mx+1);
    tetro_tetromino = 0;
}
void clean_tetro_62 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x, y, x+20, y, x+40, y, x+20, y+20,
		  my, mx, my, mx+1, my, mx+2, my+1, mx+1);
}
int can_tetro_62 (int mx, int my)
{
    return (can_tetro_xx(mx+2, my+2, screen_matrix[my][mx+2],
				       0,
				       0,
				       0));
}
int check_tetro_62 (int mx, int my)
{
    if ((screen_matrix[my][mx] == 1) ||
	(screen_matrix[my][mx+2] == 1) ||
	(screen_matrix[my+1][mx+1] == 1))
	    return 0;
    else
	return 1;
}
int tetro_62 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 2, x);
    return (tetro_xx(x, y, mx, my, 1,
	    screen_matrix[my][mx+1], screen_matrix[my][mx+2], screen_matrix[my+1][mx+1], 0,
	    screen_matrix[my][mx], screen_matrix[my][mx+1], screen_matrix[my+1][mx+1], 0,
	    clean_tetro_62, draw_tetro_62, check_tetro_62));
}

/* *
   **
   *
 */
void draw_tetro_63 (int x, int y, int mx, int my)
{
    draw_tetro_xx(x, y, x, y+20, x+20, y+20, x, y+40,
		  my, mx, my+1, mx, my+1, mx+1, my+2, mx);
}
void clean_tetro_63 (int x, int y, int mx, int my)
{
    clean_tetro_xx(x, y, x, y+20, x+20, y+20, x, y+40,
		  my, mx, my+1, mx, my+1, mx+1, my+2, mx);
}
int can_tetro_63 (int mx, int my)
{
    return (can_tetro_xx(mx+1, my+3, screen_matrix[my][mx],
				       screen_matrix[my+1][mx],
				       screen_matrix[my+1][mx+1],
				       screen_matrix[my+2][mx]));
}
int check_tetro_63 (int mx, int my)
{
    if ((screen_matrix[my+1][mx+1] == 1) ||
	(screen_matrix[my+2][mx] == 1))
	    return 0;
    else
	return 1;
}
int tetro_63 (int *x, int *y, int mx, int my)
{
    tetro_x_xx (&mx, 1, x);
    return (tetro_xx(x, y, mx, my, 2,
	    screen_matrix[my+1][mx], screen_matrix[my+1][mx+1], screen_matrix[my+2][mx], 0,
	    screen_matrix[my][mx], screen_matrix[my+1][mx], screen_matrix[my+2][mx], 0,
            clean_tetro_63, draw_tetro_63, check_tetro_63));
}

int draw_tetro (int *x, int *y, int n)
{
    int mx = ((*x)-GAME_AREA_X-10)/20;
    int my = ((*y)-GAME_AREA_Y-10)/20;
    
    switch (n) {
	case 0 : return (tetro_00(x, y, mx, my));
	case 1 : return (tetro_01(x, y, mx, my));
	case 10 : return (tetro_10(x, y, mx, my));
	case 20 : return (tetro_20(x, y, mx, my));
	case 21 : return (tetro_21(x, y, mx, my));
	case 30 : return (tetro_30(x, y, mx, my));
	case 31 : return (tetro_31(x, y, mx, my));
	case 40 : return (tetro_40(x, y, mx, my));
	case 41 : return (tetro_41(x, y, mx, my));
	case 42 : return (tetro_42(x, y, mx, my));
	case 43 : return (tetro_43(x, y, mx, my));
	case 50 : return (tetro_50(x, y, mx, my));
	case 51 : return (tetro_51(x, y, mx, my));
	case 52 : return (tetro_52(x, y, mx, my));
	case 53 : return (tetro_53(x, y, mx, my));
	case 60 : return (tetro_60(x, y, mx, my));
	case 61 : return (tetro_61(x, y, mx, my));
	case 62 : return (tetro_62(x, y, mx, my));
	case 63 : return (tetro_63(x, y, mx, my));
    }
}

void change_tetro (int x, int y, int *n)
{
    int mx = (x-GAME_AREA_X-10)/20;
    int my = (y-GAME_AREA_Y-10)/20;

    if ((*n) == 0) {
	if (can_tetro_01(mx, my)) {
	    (*n) = 1;
	    clean_tetro_00(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 1) {
	if (can_tetro_00(mx, my)) {
	    (*n) = 0;
	    clean_tetro_01(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 10) {
	return;
    }
    if ((*n) == 20) {
	if (can_tetro_21(mx, my)) {
	    (*n) = 21;
	    clean_tetro_20(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 21) {
	if (can_tetro_20(mx, my)) {
	    (*n) = 20;
	    clean_tetro_21(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 30) {
	if (can_tetro_31(mx, my)) {
	    (*n) = 31;
	    clean_tetro_30(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 31) {
	if (can_tetro_30(mx, my)) {
	    (*n) = 30;
	    clean_tetro_31(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 40) {
	if (can_tetro_41(mx, my)) {
	    (*n) = 41;
	    clean_tetro_40(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 41) {
	if (can_tetro_42(mx, my)) {
	    (*n) = 42;
	    clean_tetro_41(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 42) {
	if (can_tetro_43(mx, my)) {
	    (*n) = 43;
	    clean_tetro_42(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 43) {
	if (can_tetro_40(mx, my)) {
	    (*n) = 40;
	    clean_tetro_43(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 50) {
	if (can_tetro_51(mx, my)) {
	    (*n) = 51;
	    clean_tetro_50(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 51) {
	if (can_tetro_52(mx, my)) {
	    (*n) = 52;
	    clean_tetro_51(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 52) {
	if (can_tetro_53(mx, my)) {
	    (*n) = 53;
	    clean_tetro_52(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 53) {
	if (can_tetro_50(mx, my)) {
	    (*n) = 50;
	    clean_tetro_53(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 60) {
	if (can_tetro_61(mx, my)) {
	    (*n) = 61;
	    clean_tetro_60(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 61) {
	if (can_tetro_62(mx, my)) {
	    (*n) = 62;
	    clean_tetro_61(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 62) {
	if (can_tetro_63(mx, my)) {
	    (*n) = 63;
	    clean_tetro_62(xold, yold, mxold, myold);
	}
	return;
    }
    if ((*n) == 63) {
	if (can_tetro_60(mx, my)) {
	    (*n) = 60;
	    clean_tetro_63(xold, yold, mxold, myold);
	}
	return;
    }
    return;
}

void is_ok_to_clear (void)
{
    int i, h, w, clr=0, mltply = 0;
    
    for (h=H-1; h>=0; h--)
        if (screen_matrix[h][0] & screen_matrix[h][1] & screen_matrix[h][2] & screen_matrix[h][3] & screen_matrix[h][4] & screen_matrix[h][5] & screen_matrix[h][6] & screen_matrix[h][7] & screen_matrix[h][8] & screen_matrix[h][9]) {
	    // ok clear matrix
	    for (i=h; i>0; i--)
		for (w=0; w<W; w++) {
	    	    screen_matrix[i][w] = screen_matrix[i-1][w];
		    if (i==1)
			screen_matrix[i-1][w] = 0;
		}
	    clr = 1;
	    mltply++;
	    h++;
	}
    if (clr) {
	// yess, clear screen
        gl_fillbox(GAME_AREA_X+5, GAME_AREA_Y+5, GAME_AREA_W-8, GAME_AREA_H-8, 0);
        for (h=0; h<H; h++)
	    for (w=0; w<W; w++)
	        if (screen_matrix[h][w])
	            draw_button_area0(w*20+GAME_AREA_X+10, h*20+GAME_AREA_Y+10, 20, 20);
	score+=(100*level*mltply);
    }
}

void draw_game_area (char *buf, char *buf2, int m)
{
    int w, h;
    score = 0;
    level = 1;
    // game area
    gl_getbox(GAME_AREA_X, GAME_AREA_Y, GAME_AREA_W, GAME_AREA_H+33, buf);
    draw_button_area1(GAME_AREA_X, GAME_AREA_Y, GAME_AREA_W, GAME_AREA_H);
    gl_fillbox(GAME_AREA_X+5, GAME_AREA_Y+5, GAME_AREA_W-8, GAME_AREA_H-8, 0);
    // inst.. area
    gl_getbox(INST_AREA_X, INST_AREA_Y, INST_AREA_W, INST_AREA_H, buf2);
    draw_button_area1(INST_AREA_X, INST_AREA_Y, INST_AREA_W, INST_AREA_H);
    gl_fillbox(INST_AREA_X+5, INST_AREA_Y+5, INST_AREA_W-8, INST_AREA_H-8, 0);
    _drawtext(INST_AREA_X+15, INST_AREA_Y+15, gl_rgbcolor(128, 128, 128), helvB10, LEGEND, 180, 120, FONTNOSCROLL, 1);
    _drawtext(INST_AREA_X+30, INST_AREA_Y+40, gl_rgbcolor(128, 128, 128), helvB10, TETRIS_LEGEND, 180, 120, FONTNOSCROLL, 1);
    _drawtext(INST_AREA_X+55, INST_AREA_Y+165, gl_rgbcolor(128, 128, 128), helvR08, TETRIS_HINT, 180, 20, FONTNOSCROLL, 1);
    // first clean up
    if (m) {
        for (h=0; h<H; h++)
    	    for (w=0; w<W; w++)
		screen_matrix[h][w] = 0;
    } else {
	for (h=0; h<H; h++)
	    for (w=0; w<W; w++)
	        if (screen_matrix[h][w])
	            draw_button_area0(w*20+GAME_AREA_X+10, h*20+GAME_AREA_Y+10, 20, 20);
    }
}

int what_is_n (int rn)
{
    switch (rn) {
	case 1: return 0;
	case 2: return 1;
	case 3: return 10;
	case 4: return 20;
	case 5: return 21;
	case 6: return 30;
	case 7: return 31;
	case 8: return 40;
	case 9: return 41;
	case 10: return 42;
	case 11: return 43;
	case 12: return 50;
	case 13: return 51;
	case 14: return 52;
	case 15: return 53;
	case 16: return 60;
	case 17: return 61;
	case 18: return 62;
	case 19: return 63;
    }
}

#define draw_tetris_matrix() draw_game_area(buf, buf2, 0)

void game_tetris (void)
{
    int n, r=1, k, score_k, level_counter=0;
    int x = GAME_AREA_X+70, y = GAME_AREA_Y+10;
    char *buf, *buf2, *buf3;
    char score_text[100];
    xold = x; yold = y;
    game_minimized = 0;
    
    buf = (char *) calloc(2, GAME_AREA_W * (GAME_AREA_H+33));
    buf2 = (char *) calloc(2, INST_AREA_W * INST_AREA_H);
    buf3 = (char *) calloc(2, 50 * 20);
    draw_game_area(buf, buf2, 1);
    
    gl_getbox(220, 460, 50, 20, buf3);
    draw_button(220, 460, "Tetris", 1);
        
    // wait for start
    k = _drawtext(INST_AREA_X+(INST_AREA_W-k)/2, INST_AREA_Y+185, gl_rgbcolor(255, 0, 0), helvB10, PRESS_START, 180, 20, FONTNOSCROLL, 0);
    gl_fillbox(INST_AREA_X+(INST_AREA_W-k)/2, INST_AREA_Y+185, k, 20, 0);
    _drawtext(INST_AREA_X+(INST_AREA_W-k)/2, INST_AREA_Y+185, gl_rgbcolor(255, 0, 0), helvB10, PRESS_START, 180, 20, FONTNOSCROLL, 1);
    while (1) {
	UPDATERS(1);
	games_minimize(PRESS_START, "Tetris", draw_tetris_matrix(), 33);
	if (key == 'r')
	    break;
	if (key == 'q') {
	    quit_game(33);
	    return;
	}
    }
    gl_fillbox(INST_AREA_X+(INST_AREA_W-k)/2, INST_AREA_Y+185, k, 20, 0);

    // started
    while (1) {
	UPDATERS(0);
	n = what_is_n(_rand(19));
	level_counter++;
	if (level_counter==20) {
	    level++;
	    level_counter=0;
	}
	if (screen_matrix[0][0] | screen_matrix[0][1] | screen_matrix[0][2] | screen_matrix[0][3] | screen_matrix[0][4] | screen_matrix[0][5] | screen_matrix[0][6] | screen_matrix[0][7] | screen_matrix[0][8] | screen_matrix[0][9]) {
	    k = _drawtext(INST_AREA_X+55, INST_AREA_Y+160, gl_rgbcolor(255, 0, 0), helvB10, GAME_OVER, 180, 20, FONTNOSCROLL, 0);
	    _drawtext(INST_AREA_X+(INST_AREA_W-k)/2, INST_AREA_Y+185, gl_rgbcolor(255, 0, 0), helvB10, GAME_OVER, 180, 20, FONTNOSCROLL, 1);
	    // score
	    sprintf(score_text, "Score %d / Level %d", score, level);
	    score_k = _drawtext(GAME_AREA_X+(GAME_AREA_W-k)/2, GAME_AREA_Y+185, gl_rgbcolor(255, 0, 0), helvB10, score_text, 180, 20, FONTNOSCROLL, 0);
	    if (score_k>200)
		score_k = 200;
	    gl_fillbox(GAME_AREA_X+(GAME_AREA_W-k)/2, GAME_AREA_Y+184, k, 20, 0);
	    _drawtext(GAME_AREA_X+(GAME_AREA_W-score_k)/2, GAME_AREA_Y+185, gl_rgbcolor(255, 0, 0), helvB10, score_text, 200, 20, FONTNOSCROLL, 1);
	    while (1) {
		UPDATERS(1);
		games_minimize(GAME_OVER, "Tetris", draw_tetris_matrix(), 33);
	        switch (key) {
		    case 'q' :
			quit_game(33);
	    		return;
    		    case 'r' :
			quit_game(33);
			game_tetris();
			return;
		}
	    }
	} else {
	    r = 1;
	}
	while (r) {
	    sprintf(score_text, "%d / %d", score, level);
	    score_k = _drawtext(INST_AREA_X+(INST_AREA_W-k)/2, INST_AREA_Y+185, gl_rgbcolor(255, 0, 0), helvB10, score_text, 180, 20, FONTNOSCROLL, 0);
	    if (score_k>200)
		score_k = 200;
	    gl_fillbox(INST_AREA_X+(INST_AREA_W-k)/2, INST_AREA_Y+185, k, 20, 0);
	    _drawtext(INST_AREA_X+(INST_AREA_W-score_k)/2, INST_AREA_Y+185, gl_rgbcolor(255, 0, 0), helvB10, score_text, 200, 20, FONTNOSCROLL, 1);
	    UPDATERS(0);
	    switch (key) {
		case 'q' :
		    quit_game(33);
		    return;
		case 'r' :
		    quit_game(33);
		    game_tetris();
		    return; break;
		case 'p' :
		    k = _drawtext(INST_AREA_X+(INST_AREA_W-k)/2, INST_AREA_Y+185, gl_rgbcolor(255, 0, 0), helvB10, PAUSE, 180, 20, FONTNOSCROLL, 0);
		    gl_fillbox(INST_AREA_X+(INST_AREA_W-k)/2, INST_AREA_Y+185, k, 20, 0);
		    _drawtext(INST_AREA_X+(INST_AREA_W-k)/2, INST_AREA_Y+185, gl_rgbcolor(255, 0, 0), helvB10, PAUSE, 180, 20, FONTNOSCROLL, 1);
		    while (1) {
			UPDATERS(1);
			games_minimize(PAUSE, "Tetris", draw_tetris_matrix(), 33);
			if (key == 'p')
			    break;
		    }
		    gl_fillbox(INST_AREA_X+(INST_AREA_W-k)/2, INST_AREA_Y+185, k, 20, 0);
		    break;
		case 'm' :
		    game_minimized = 1; break;
		case '4' :
		    x-=20; break;
		case '6' :
		    x+=20; break;
		case '5' :
		    change_tetro(x, y, &n); break;
	    }
	    games_minimize("", "Tetris", draw_tetris_matrix(), 33);
	    r = draw_tetro(&x, &y, n);
	    y+=20;
	    if (n == 62) // tetro 62
		score += 20*level;
	    else
	        score += 10*level;
	    usleep(250000-((level-1)*20000));
	    gl_fillbox(INST_AREA_X+(INST_AREA_W-k)/2, INST_AREA_Y+185, k, 20, 0);
	}
	is_ok_to_clear();
	x = GAME_AREA_X+70, y = GAME_AREA_Y+10;
    }
}
