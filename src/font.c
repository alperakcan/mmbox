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
#include <vga.h>
#include <vgagl.h>
#include <stdio.h>
#include <stdlib.h>

#include "fonts/fontbmps.h"
#include "font.h"
#include "utils.h"

/* Writes font properties to the given struct.
 * c, color
 * f, font type
 * stuct to hold font properties.
 */
struct fontinfo *setfont (int c, int f, struct fontinfo *font)
{
    switch (f) {
	case helvR08 :
	    font->fontdat=font1dat;
	    font->fonttbl=font1tbl;
	    font->fontfh=font1fh;
	    font->fontyofs=font1yofs;
	    font->fontoy=font1oy;
	    font->color=c;
	    break;
	case helvR10 :
	    font->fontdat=font2dat;
	    font->fonttbl=font2tbl;
	    font->fontfh=font2fh;
	    font->fontyofs=font2yofs;
	    font->fontoy=font2oy;
	    font->color=c;
	    break;
	case helvB10 :
	    font->fontdat=font3dat;
	    font->fonttbl=font3tbl;
	    font->fontfh=font3fh;
	    font->fontyofs=font3yofs;
	    font->fontoy=font3oy;
	    font->color=c;
	    break;
    }
    return font;
}

/*
 * if int draw == 0
 *	 just calculates the width and height of the area,
 * if int draw == 1
 * 	 calculates the w, h of the text area, and draws the text. 	 
 * that is needed to show text->string
 *
 * Why we need this stupity? ;) We need to scroll, huh!
 */
struct textinfo *drawfontarea (struct fontinfo *font, struct textinfo *text, int draw)
{
    int f,c;
    int xpos=0, ypos=0;
    int xposmax=xpos;
    int xpostmp=xpos;
    int ypostmp=ypos;
    signed char *ptr;
    
    int ox, oy, x, y, w, h, dw, yst;
    
    for (f=0; f<strlen(text->string); f++) {
	c=text->string[f];

	if (c=='\t')
	    c = ' ';
	
	ypostmp = ypos;

	if (c=='\n') {
	    if (text->scroll==FONTNOSCROLL || text->scroll==FONTFORCEWIDTH)
		if ((ypostmp+2*font->fontfh>text->height) && (text->height != (int)NULL))
		    break;

	    ypos += font->fontfh;
	    xpos = 0;

	    if (xpos>xposmax)
		xposmax=xpos;

	    continue;
	}

	switch (c) {
	    case 'Ç' : // tr c big
	        c = 127; break;
	    case 'Ö' : // tr o big
		c = 128; break;
	    case 'Ü' : // tr u big
		c = 129; break;
	    case 'ç' : // tr c small
		c = 130; break;
	    case 'ö' : // tr o small
		c = 131; break;
	    case 'ü' : // tr u small
		c = 132; break;
	    case 'Þ' : // tr s big
		c = 133; break;
	    case 'þ' : // tr s small
		c = 134; break;
	    case 'Ý' : // tr i big
		c = 135; break;
	    case 'ý' : // tr i small
		c = 136; break;
	    default :
		if (c<32 || c>136)
		    c = '_';
	}
	
	ptr = font->fontdat+font->fonttbl[c-32];
	ox = *ptr++;	oy = *ptr++;
	w = *ptr++;	h = *ptr++;
	dw = *ptr++;
	yst=font->fontfh-(oy-font->fontoy)-font->fontyofs-h;
	
	xpostmp = xpos;
	xpostmp+=dw;

        if (text->scroll==FONTNOSCROLL) {
	    if (xpostmp>text->width) {
		for (;f<strlen(text->string); f++)
		    if (text->string[f] == '\n') {
			f--;
		    	break;
		    }
		continue;
	    }
	    if ((ypostmp+font->fontfh>text->height) && (text->height != (int)NULL))
	        break;
	}
	
	if (text->scroll==FONTFORCEWIDTH) {
	    if (xpostmp>text->width) {
		xpos=0;
	        ypostmp+=font->fontfh;
	    }
	    if ((ypostmp+font->fontfh>text->height) && (text->height != (int)NULL))
	        break;
	}

	ypos = ypostmp;

	if (draw)
            for (y=yst;y<yst+h;y++) {
    		for (x=ox;x<ox+w;x++)
	    	    if (*ptr++)
			gl_setpixel(xpos+x+text->xpos, ypos+y+text->ypos, font->color);
    	    }

	xpos+=dw;

	if (xpos>xposmax)
	    xposmax=xpos;
    }

    text->cwidth = xposmax;
    if (!xposmax)
        text->cheight = ypos;
    else
	text->cheight = ypos+font->fontfh;
    return (text);
}

/* x -> x position
 * y -> y position
 * c -> color
 * f -> font
 * *s -> string
 * w -> width
 * h -> height
 * s -> scroll type
 */
int _drawtext (int x, int y, int c, int f, char *str, int w, int h, int s, int d)
{
    struct fontinfo *font;
    struct textinfo *text;
    int ret;

    font = (struct fontinfo *)calloc(1, sizeof(struct fontinfo));
    text = (struct textinfo *)calloc(1, sizeof(struct textinfo));

#ifdef DEBUG
    if (!(font && text))
	PANIC;
#endif

    font = setfont(c, f, font);
    text->string = str;
    text->scroll = s;
    text->width = w;
    text->height = h;
    text = drawfontarea(font, text, 0);
    
    if (d == 0) { // just cacl. & return
	ret = text->cwidth;
        font = NULL;
	text = NULL;
	free(font);
	free(text);
	return ret;
    }
    
    text->xpos = x;
    text->ypos = y;
    text = drawfontarea(font, text, 1);
    
    font = NULL;
    text = NULL;
    free(font);
    free(text);
}
