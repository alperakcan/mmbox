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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vgagl.h>
#include <vgamouse.h>

#include "irman.h"
#include "ir.h"
#include "../main.h"
#include "../mouse.h"
#include "../font.h"
#include "../utils.h"
#include "../kbd.h"

int irman_init (void)
{
    int ret;

    ret = ir_open_port(IRMAN_DEVICE);
    if (ret == -1)
	return -1;

    ret = ir_init(IRMAN_DEVICE);
    if (ret == -1)
	return -1;

    return 1;
}

int irman_getcode (void)
{
    int ret, i;
    unsigned char *code;
    char *code_text;

    code = ir_poll_code();
    if (code == NULL)
	return -1;

    code_text = ir_code_to_text(code);

    ret = ir_valid_code(code_text);
    if (ret == 0)
	return -1;

    for (i=0; i<IRCODES; i++) {
	ret = strncmp(code_text, ir_codes[i], 12);
	if (ret == 0)
	    return i;
    }
    ir_clear_buffer();
}

int irman_uninit (void)
{
    ir_finish();
}

void ir_setcommands (void)
{
    unsigned char *code;
    char *code_text;
    int ret, i, j, k, l;
    int w = (10+50+50+100+10), h = (20+245+20+55);
    int x=(WIDTH-w)/2, xb=x+150, y=(HEIGHT-h)/2;
    char buf[w*h*2];
    char code_buf[13];
    char *code_texts[IRCODES] = {IR_CODE_TEXTS};
    
    draw_box0(x-10, y-20, w, h, IRMAN_CODES_TEXT, buf);

    for (i=0, j=0; i<IRCODES; i++, j+=20)
        draw_button(xb, y+j, code_texts[i], 0);
    
    for (i=0,j=0; i<IRCODES; i++, j+=20) {
	draw_button_area3(x, y+2+j, 100, 14);
	snprintf(code_buf, 13, "%s", ir_codes[i]);
	_drawtext(x+15, y+4+j, 0, helvR08, code_buf, 208, 23, FONTNOSCROLL, 1);
    }
    _drawtext(x, y+4+j, 0, helvR08, IRMAN_HINT, 180, 43, FONTFORCEWIDTH, 1);
    j+=30;
    draw_button(x-10+(w-50)/2, y+4+j, TEXT_OK, 0);
    k = j;

    while (1) {
	UPDATERS(1);
	for (i=0, j=0; i<IRCODES; i++, j+=20) {
	    if_mouse_pressed_over(xb, y+j, 50, 19, MOUSE_LEFTBUTTON) {
		draw_button(xb, y+j, code_texts[i], 1);
		gl_fillbox(x, y+4+k-30, 180, 30, COL_BUTTON_3);
		_drawtext(x, y+4+k-30, 0, helvR08, IRMAN_HINT2, 185, 43, FONTFORCEWIDTH, 1);
//	        code = ir_get_code();
		code = ir_get_code_time(1000000*5);
		if (code == NULL)
		    goto IR_ERROR;
	        code_text = ir_code_to_text(code);
	        ret = ir_valid_code(code_text);
	        if (ret == 0) {
IR_ERROR:
		    info_box(IRMAN_CODES_TEXT, IRMAN_NO_CODE);
		    draw_button(xb, y+j, code_texts[i], 0);
		    gl_fillbox(x, y+4+k-30, 185, 30, COL_BUTTON_3);
		    _drawtext(x, y+4+k-30, 0, helvR08, IRMAN_HINT, 180, 43, FONTFORCEWIDTH, 1);
		}
	        else {
		    for (l=0; l<IRCODES; l++)
			if (strncmp(code_text, ir_codes[l], 12) == 0)
			    sprintf(ir_codes[l], "00000000000");
		    for (l=0; l<12; l++) {
		        ir_codes[i][l] = code_text[l];
		    }
		    gl_putbox(x-10, y-20, w, h, buf);
		    ir_setcommands();
		    return;
		}
	    }
	}
        if_mouse_pressed_over(x-10+(w-50)/2, y+4+k, 50, 19, MOUSE_LEFTBUTTON) {
	    goto QUIT_IR;
	}
	if (key == RK_ESC) {
QUIT_IR:
	    draw_button(x-10+(w-50)/2, y+4+k, TEXT_OK, 1);
	    gl_putbox(x-10, y-20, w, h, buf);
	    break;
	}
    }
}
