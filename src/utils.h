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
/* signals */
#define SIG_CDKILL	 SIGRTMIN
#define SIG_CDPLIST	(SIGRTMIN+1)
#define SIG_MP3KILL	(SIGRTMIN+2)
#define SIG_MP3PLIST	(SIGRTMIN+3)
#define SIG_UPDTR_VERBOSE	(SIGRTMIN+4)
#define SIG_PSBR_VERBOSE	(SIGRTMIN+5)
#define SIG_SANLYZR_COLOR	(SIGRTMIN+6)

struct popup {
    int i;
    char item[5][20];
};

#ifdef DEBUG

#define PANIC (panic(__FILE__, __LINE__))
void panic (char *filename, int line);

#define TELLME (TEllME(__FILE__, __LINE__))
void TEllME (char *filename, int line);

#endif

/*
 * returns a randomized number between 1 & given max.
 */
int _rand (int max);

/*
 * sets sigaction, when recieved signum call func)
 * sigset(signum, function
 */
void sigset (int signum, void(func)(int foo));

/*            . (x0, y0)
 *
 *       c0       c2
 *
 *  . (x1, y1)  c1   . (x2, y2)
 */
void _gl_triangle (int x0, int y0, int c0, int x1, int y1, int c1, int x2, int y2, int c2);

/*
 *   _______________________________
 *  |  ___________________________| |
 *  | |  _______________________| | |
 *  | | |                       | | |
 *  |0|2|3  Simple Button Area  | | |
 *  | |_|_______________________| | |
 *  |_|1__________________________| |
 *  |__4____________________________|
 *
 */ 
void draw_button_area0 (int x, int y, int w, int h);
// bonus ;)
void draw_tetro_area0 (int x, int y, int w, int h);
/* And pressed.
 */
void draw_button_area1 (int x, int y, int w, int h);

/*
 *   _______________________________
 *  |  ___________________________| |
 *  | |                           | |
 *  | |                           | |
 *  |0|     Button Area 2         | |
 *  | |                           | |
 *  |_|___________________________| |
 *  |__4____________________________|
 *
 */ 
void draw_button_area2 (int x, int y, int w, int h);
/* And pressed
 */
void draw_button_area3 (int x, int y, int w, int h);
void draw_button_area31 (int x, int y, int w, int h);

/*
 *   _______________________________
 *  |_______________________________|
 *  |                               |
 *  |                               |
 *  |       Box  0                  |
 *  |                               |
 *  |_______________________________|
 *
 */ 
void draw_box0 (int x, int y, int w, int h, char *toptext, char *buf);
/* Simple info box, with using box0
 * tstr => top text,
 * str => definition
 */
void info_box (char *tstr, char *str);
// ok cancel box, return 1, or 0.
int question_box (char *tstr, char *str);

/*
 * slider ;)
 */
void draw_slider (int x, int y, int h, int parts, int part);

/*
 * p => 1 pressed
 * p => 0 virgin ;)
 * h = 20;
 * w = 50.
 */
void draw_button (int x, int y, char *str, int p);

/* draw popup menu ;)
    popup = (struct popup *) calloc(1, sizeof(struct popup));
    popup->i = 4;
    popup->item = (char **)calloc(1, popup->i);
    for(i = 0; i<popup->i; i++)
	popup->item[i] = (char *)calloc(sizeof(char), NAME_MAX+1);
    popup->item[0] = "*";
    popup->item[1] = "*.mp3";
    popup->item[2] = "*.mp2";
    popup->item[3] = "*.mp1";
    matchi = draw_popup(X, Y-(15+4+13*(popup->i)), "Match", popup);
    free(popup);
 */
int draw_popup (int x, int y, char *tstr, struct popup *tmp);

void write_ppm (void);
