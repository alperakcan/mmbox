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
#include <stdlib.h>
#include <unistd.h>

#include "../font.h"
#include "../main.h"
#include "updater.h"
#include "sanalyzer.h"
#include "libcda/cda.h"

static int i = 0;
static int j = 0;
static int minutes = 0;
static int seconds = 0;
static int bar_percent = 0;
static int first_time = 1;

void updater_title_clear (void)
{
    if (!updtr_verbose) {
	gl_fillbox(55, 250, 214, 130, COL_UPDATER_BACK);
	gl_fillbox(55, 370, 150, 15, COL_UPDATER_BACK);
    }
    // posbar
    if (!posbar_verbose) {
        gl_fillbox(301, 422, 246, 18, 0);
	gl_putboxmask(301, 422, 246, 18, posbar);
    }
}

void updater_posbar (int totsiz, int bdone, int br)
{
    int min, sec, m, s, k, c;
    int pbar_percnt = 0;
    char text[37];
    struct msf msf;
    
    pbar_percnt = (245 * bdone) / totsiz;
    if (br > 0) { // mp3
	sec = (totsiz-bdone) / (br * 128);
	s = bdone / (br * 128);
	min = sec/60;
	sec = sec%60;
	m = s/60;
	s = s%60;
    } else { // cd
        cd_lba2msf((totsiz-bdone), &msf);
	min = msf.minutes;
	sec = msf.seconds;
	cd_lba2msf(bdone, &msf);
	m = msf.minutes;
	s = msf.seconds;
    }
    
    if (!updtr_verbose) {
        _drawtext(62+10, 290, gl_rgbcolor(128, 128, 128), helvR08, TEXT_ELAPSED, 208, 23, FONTNOSCROLL, 1);
	_drawtext(62+208/2, 290, gl_rgbcolor(128, 128, 128), helvR08, TEXT_REMAINING, 208/2, 23, FONTNOSCROLL, 1);

	if (seconds!=s || minutes != m) {
    	    sprintf(text, "%02d : %02d               %02d : %02d", m, s, min, sec);
	    gl_fillbox(62, 310, 208, 20, COL_UPDATER_BACK);
	    k = _drawtext(62, 310, COL_UPDATER_TEXT, helvB10, text, 208, 23, FONTNOSCROLL, 0);
	    _drawtext(62+((208-k)/2), 310, COL_UPDATER_TEXT, helvB10, text, 208, 23, FONTNOSCROLL, 1);
	    minutes = m;
	    seconds = s;
	}
    }

    if (!posbar_verbose) {
        if ((bar_percent!=pbar_percnt) && (pbar_percnt<=245) && (pbar_percnt>=0)) {
/*	    // for faster updates
	    gl_fillbox(302+bar_percent, 422, 245-bar_percent, 18, 0);
	    for (k=bar_percent; k<pbar_percnt; k++) {
		c = gl_rgbcolor(250-k, 0, k+5);
		gl_line(302+k, 422, 302+k, 422+17, c);
	    }
*/	    // we should print all, becouse of console swtiches
	    gl_fillbox(301, 422, 246, 18, 0);
	    for (k=0; k<pbar_percnt; k++) {
		c = gl_rgbcolor(250-k, 0, k+5);
		gl_line(302+k, 422, 302+k, 422+17, c);
	    }
	    sprintf(text, "%02d%%", (pbar_percnt*100)/245);
	    k = _drawtext(62, 310, COL_UPDATER_TEXT, helvB10, text, 208, 23, FONTNOSCROLL, 0);
	    _drawtext(301+(246-k)/2, 422, gl_rgbcolor(120,120,120), helvB10, text, 208, 23, FONTNOSCROLL, 1);
	    gl_putboxmask(301, 422, 246, 18, posbar);
	    bar_percent = pbar_percnt;
	}
    }
}

void updater_printf_infos (char *st, int br, int fr)
{
    int k, k_br, k_hz;
    char text[100];
    
    if ((!updtr_verbose) && (first_time == 1)) {
        gl_fillbox(62, 345, 208, 35, COL_UPDATER_BACK);
	if (br!=-1)
    	    sprintf(text, "%d ", br);
	else
	    sprintf(text, "14H ");
	k_br = _drawtext(62, 345, COL_UPDATER_TEXT, helvR10, text, 208, 23, FONTNOSCROLL, 0);
	sprintf(text, "%d ", fr);
	k_hz = _drawtext(62, 345, COL_UPDATER_TEXT, helvR10, text, 208, 23, FONTNOSCROLL, 0);
	if (br!=-1)
    	    sprintf(text, "%d Kbits/s", br);
        else
	    sprintf(text, "14H Kbits/s");
	_drawtext(62+10, 345, COL_UPDATER_TEXT, helvR10, text, 208, 23, FONTNOSCROLL, 1);
	_drawtext(62+10+k_br, 345, gl_rgbcolor(90, 90, 90), helvR10, "Kbits/s", 208, 23, FONTNOSCROLL, 1);
	sprintf(text, "%d Khz", fr);
	k = _drawtext(62, 345, COL_UPDATER_TEXT, helvR10, text, 208, 23, FONTNOSCROLL, 0);
	_drawtext(62+208-k-10, 345, COL_UPDATER_TEXT, helvR10, text, 208, 23, FONTNOSCROLL, 1);
	_drawtext(62+208-k-10+k_hz, 345, gl_rgbcolor(90, 90, 90), helvR10, "Khz", 208, 23, FONTNOSCROLL, 1);
	k = _drawtext(62, 365, COL_UPDATER_TEXT, helvR10, st, 208, 23, FONTNOSCROLL, 0);
	_drawtext(62+((208-k)/2), 365, COL_UPDATER_TEXT, helvR10, st, 208, 23, FONTNOSCROLL, 1);
	first_time = 0;
    }
}

void updater_slide_file_name (char *name)
{
    if (j==0) {
	if (name[i] == '\0')
	    i = 0;
        gl_fillbox(55, 255, 209, 18, COL_UPDATER_BACK);
	_drawtext(55, 255, COL_UPDATER_TEXT, helvB10, name+i, 208, 23, FONTNOSCROLL, 1);
	i++;
    }
    j++;
    if (j==6)
	j=0;
}

void updater (short p_data[2][512], int chans, char *name, int fsiz, int b_done, int br)
{
    if (!updtr_verbose) {
        updater_slide_file_name(name);
	sanalyzer(p_data, chans);
    }
    if (!posbar_verbose) {
	updater_posbar(fsiz, b_done, br);
    }
}

void updater_verbose_onoff (int foo)
{
    if (updtr_verbose == 1)
	updtr_verbose = 0;
    else
	updtr_verbose = 1;
}
void posbar_verbose_onoff (int foo)
{
    if (posbar_verbose == 1)
	posbar_verbose = 0;
    else
	posbar_verbose = 1;
}
