/*
    Based on libirman, see README.libirman
*/
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
#define IRCODES 13

#define RI_QUIT 0
#define RI_PLAY 1
#define RI_PAUSE 2
#define RI_STOP 3
#define RI_NEXT 4
#define RI_PREV 5
#define RI_SHUFFLE 6
#define RI_REPLAY 7
#define RI_JBOX 8
#define RI_JBOXMP3 9
#define RI_JBOXCD 10
#define RI_VOL_UP 11
#define RI_VOL_DOWN 12

char ir_codes[IRCODES][12];

int irman_init (void);

int irman_getcode (void);

int irman_uninit (void);

void ir_setcommands (void);
