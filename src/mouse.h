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
 * vga_mousesupport(1)
 * mouse_setscale(?) the larger the scale, the slower the mouse.
 * mouse_setposition(int x, int y)
 *
 * while (!quit) 
 * {
 * 	_mouse_update()
 * 	.....
 * }
 */

/*
 * _mouse_waitbuttonrealese();
 * simple, no need to describe ;)
 */

/* a => x
 * b => y
 * w => width
 * h => height
 * c => button
 */
#define if_mouse_over(a, b, w, h) if ((mouse.x >= a) &&\
				     (mouse.x <= (a + w)) &&\
				     (mouse.y >= b) &&\
				     (mouse.y <= (b + h)))

#define if_mouse_pressed_over(a, b, w, h, c) if ((mouse.x >= a) &&\
					          (mouse.x <= (a + w)) &&\
						  (mouse.y >= b) &&\
						  (mouse.y <= (b + h)) &&\
						  ((mouse.button) & c))

#define if_mouse_clicked_over(a, b, w, h, c) if ((mouse.x >= a) &&\
					          (mouse.x <= (a + w)) &&\
						  (mouse.y >= b) &&\
						  (mouse.y <= (b + h)) &&\
						  ((mouse.clickedbutton) & c))
typedef struct {
    int x;
    int y;
    int ox;
    int oy;
    int button;
    int obutton;
    int oobutton;
    int clickedbutton;
} MOUSE;
MOUSE mouse;

void _mouse_update (void);

void _mouse_waitbuttonrealese (void);
