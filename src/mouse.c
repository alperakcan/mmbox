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
/*
 * Idea is simple;
 *
 * 1- Take a snapshot of the section of the screen where the mouse pointer will
 *    be painted.
 * 2- Draw the pointer icon in the area where the snapshot has taken.
 * 3- When movement occurs, restore the screen area where the mouse pointer
 *    was with our snapshot. Then find the new coordinates and start over
 *    with Step 1.
 */

#include <vgagl.h>
#include <vgamouse.h>
#include <unistd.h>

#include "main.h"
#include "mouse.h"

int sleep_nano (long nsec);

/*
static unsigned char mousecursor[] =
{
"....++++++++++++"
".@@@..++++++++++"
".#@@@@..++++++++"
".##@@@@@..++++++"
"+.##@@@@@@..++++"
"+.###@@@@@@@.+++"
"++.###@@@@@@@.++"
"++.####@@@...+++"
"+++.####@@@.++++"
"+++.#####@@@.+++"
"++++.##.##@@@.++"
"++++.##..##@@@.+"
"+++++.#.+.##@@@."
"++++++.+++.##@.+"
"+++++++++++.#.++"
"++++++++++++.+++"
};
*/

static unsigned char mousecursor[] =
{
".++++++++++"
"..+++++++++"
".@.++++++++"
".@@.+++++++"
".#@@.++++++"
".#@@@.+++++"
".##@@@.++++"
".##@@@@.+++"
".###@@@@.++"
".###@@@@@.+"
".####@....."
".##.#@.++++"
".#.+.#@.+++"
"..++.#@.+++"
".++++.#@.++"
"+++++.#@.++"
"+++++++..++"
};

char mousebox[11*17*2];

static int first_time = 1;

void _mouse_update (void)
{
    int x, y;
    char *cursor = mousecursor;
    
    mouse_update();
    mouse.x = mouse_getx();
    mouse.y = mouse_gety();
    mouse.button = mouse_getbutton();
    
    /* check for double-clicked button */
    mouse.clickedbutton = 0;
    if ((mouse.button != 0) && (mouse.button == mouse.oobutton))
        if (!mouse.obutton)
	    mouse.clickedbutton = mouse.button;

    /* snapshot */
    gl_getbox(mouse.x, mouse.y, 11, 17, mousebox);
	
    /* draw mouse */
    for (y=0; y<17; y++) {
        for (x=0; x<11; x++) {
	    if (*cursor != '+') {
	        if ((mouse.x+x)<WIDTH) {
		    switch (*cursor) {
		    case '.' :
		        gl_setpixel((mouse.x+x), (mouse.y+y), gl_rgbcolor(0,0,0));break;
		    case '@' :
		        gl_setpixel((mouse.x+x), (mouse.y+y), gl_rgbcolor(255,255,255));break;
		    case '#' :
		        gl_setpixel((mouse.x+x), (mouse.y+y), gl_rgbcolor(214,211,206));break;
		    }
		}
	    }
	    cursor++;
	}
    }
    
    mouse.ox = mouse.x;
    mouse.oy = mouse.y;
    
    mouse.oobutton = mouse.obutton;
    mouse.obutton = mouse.button;

    /* restrore */
    sleep_nano((long)50000000);
    gl_putbox(mouse.ox, mouse.oy, 11, 17, mousebox);
}

void _mouse_waitbuttonrealese (void)
{
    while (1) {
	if (!mouse.button)
	    break;
	UPDATERS(1);
    }
}
