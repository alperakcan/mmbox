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
#define STOPPED 0
#define PLAYING 1
#define PAUSED 2

struct player {
    int status; // stopped, playing, paused.
    int num; // playlist number of the file.
    int shuffle; // shuffle?
    int replay; // replay?
    int pid; // self pid.
    int upid; // updater pid.
    int fd; // for cd player.
};

struct jbox {
    void (*stop) (int);
    void (*play) (int);
    void (*pause) (void);
    int (*next) (void);
    void (*prev) (void);
    void (*shuffle) (void);
    void (*replay) (void);
    void (*playlist_handler) (int k);
    void (*uninit) (void);
};
struct jbox jbox;

// jbox init
void jbox_init (void);
// jbox keyboard and mouse handler.
int jbox_handler (void);

// if i=0 -> i=1, and visewersa.
int on_off (int *i);
// function names are enough.
int boxpause (int *s, int *p);
int boxprev (int *num, int shuffle, int replay, int first, int last);
int boxnext (int *num, int shuffle, int replay, int first, int last);
int boxstop (int *stat, int *p, int *up);
void draw_shufrep(int s, int r);
// handler cases
void do_handler_case (int x, int y, int w, int h, char *img, char *box, int type);
