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
#include <vgakeyboard.h>

#include "kbd.h"
#include "main.h"

/* Got codes with this ;)
 *
 * #include <stdio.h>
 * int main ()
 * {
 * 	getchar();
 * }
*/

int kbd (void)
{
    int i;
    int k[4];
    int ret;

    for (i=0; i<4; i++)
	k[i] = vga_getkey();
    
    if (k[0] == 27 && k[1] == '[') {
	switch (k[2]) {
	    case 'A' :	return(RK_CURSOR_UP);
	    case 'B' :	return(RK_CURSOR_DOWN);
	    case 'C' :	return(RK_CURSOR_RIGHT);
	    case 'D' :	return(RK_CURSOR_LEFT);
	    case '[' :
		switch (k[3]) {
		    case 'A' :	return(RK_F1);
		    case 'B' :	return(RK_F2);
		    case 'C' :	return(RK_F3);
		    case 'D' :	return(RK_F4);
		    case 'E' :	return(RK_F5);
		} break;
	    case '1' :
		switch (k[3]) {
		    case '~' :	return(RK_HOME);
		    case '7' :	return(RK_F6);
		    case '8' :	return(RK_F7);
		    case '9' :	return(RK_F8);
		} break;
	    case '2' :
		switch (k[3]) {
		    case '~' :	return(RK_INSERT);
		    case '0' :	return(RK_F9);
		    case '1' :	return(RK_F10);
		    case '3' :	return(RK_SHIFT_F1);	// F11
		    case '4' :	return(RK_SHIFT_F2);	// F12
		} break;
	    case '3' :
		switch (k[3]) {
		    case '~' :	return(RK_DELETE);
		} break;
	    case '4' :	return(RK_END);
	    case '5' :	return(RK_PAGE_UP);
	    case '6' :	return(RK_PAGE_DOWN);
	}
    }
    // otherwise
    return(k[0]);
}
