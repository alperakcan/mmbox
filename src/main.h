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
#include "rcfile.h"
#include "main_lang.h"
#include "main_img.h"
#include "main_col.h"
#include "main_files.h"

#define FALSE	0
#define TRUE	1

#define MODE	18 /* G640x480x32K */

/* devices */
#define DSP_DEVICE cfg.dsp_device
#define MIXER_DEVICE cfg.mixer_device
#define CD_DEVICE cfg.cd_device
#define IRMAN_DEVICE cfg.irman_device

#define UPDATERS(a)	key = kbd();\
			ir = irman_getcode();\
			if (a)\
			    _mouse_update();\
			else\
			    sleep_nano((long)50000000);\
			clock_update();\
			if (key == '`')\
			    write_ppm();


static int quit;
int key;
int key_old;
int ir;

typedef struct {
    int which;
    int (*main_handler) (void);
    int (*window_handler) (void);
} HANDLER;
HANDLER handler;
