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
    USAGE:
    
    struct fontinfo *font;
    struct textinfo *text;
    font = calloc(1, sizeof(struct fontinfo));
    text = calloc(1, sizeof(struct textinfo));
#ifdef DEBUG
    if(!(font && text))
	PANIC;
#endif
    font = setfont(gl_rgbcolor(0,0,255), helvR08, font);
    text->string = "aAAas3d1 ehj\nh\nejehj\nkehkejhekjhekjhejkh 2789728";
    
    WILL NOT SCROLL:
    text->scroll = FONTNOSCROLL;
    text->width = 34;
    text->height = 50;
    
    WIL FORCE WIDTH, NOT SCROLL:
    text->scroll = FONTFORCEWIDTH;
    text->width = 34;
    text->height = 50;
    
    WILL SCROLL:
    text->scroll = FONTSCROLL;
    text->width = 0;
    text->height = 0;
    
//    if text->height == 0, this means that we will scroll y.
//    Remember: to scroll a area, it must be drawn to a virtual screen.

    text = drawfontarea(font, text, 0);
    printf("main:\n");
    printf("w %d, h%d, s%d\n", text->cwidth, text->cheight, text->scroll);
    textt->xpos = 50;
    textt->ypos = 50;
    textt = drawfontarea(font, text, 1);
    printf("main:\n");
    printf("w %d, h%d, s%d\n", text->cwidth, text->cheight, text->scroll);
    free(font);
    free(text);
    
    OR:

    _drawtext(50, 50, 31, helvR08, text->string, 34, 20, FONTNOSCROLL);
    printf("w %d, h%d, s%d\n", text->cwidth, text->cheight, text->scroll);
*/
#define helvR08		1
#define helvR10		2
#define helvB10		3

#define FONTNOSCROLL	0
#define FONTSCROLL	1
#define FONTFORCEWIDTH	2

struct fontinfo {
    signed char *fontdat;
    int *fonttbl;
    int fontfh;
    int fontyofs;
    int fontoy;
    int color;
};

struct textinfo {
    char *string;
    int cwidth;		// calculated width
    int cheight;	// calculated height
    int width;		// forced width
    int height;		// forced height
    int xpos;
    int ypos;
    int scroll;
};

/* Writes font properties to the given struct.
 * c, color
 * f, font type
 * stuct to hold font properties.
 */
struct fontinfo *setfont (int c, int f, struct fontinfo *font);

/*
 * if int draw == 0
 *	 just calculates the width and height of the area,
 * if int draw == 1
 * 	 calculates the w, h of the text area, and draws the text. 	 
 * that is needed to show text->string
 *
 * Why we need this stupity? ;) We need to scroll, huh!
 */
struct textinfo *drawfontarea (struct fontinfo *font, struct textinfo *text, int draw);

/* x -> x position
 * y -> y position
 * c -> color
 * f -> font
 * *s -> string
 * w -> width
 * h -> height
 * s -> scroll type
 */
int _drawtext (int x, int y, int c, int f, char *str, int w, int h, int s, int d);
