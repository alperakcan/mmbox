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
#include <math.h>
#include <fcntl.h>

#include "sanalyzer.h"
#include "updater.h"
#include "fft.h"
#include "../main.h"

#define SA_X 49
#define SA_Y 213
#define SA_H 73
#define SA_W 224
#define NUM_BANDS 16

#define GREENRED 0
#define BLUEWHITE 1
#define BLUERED 2

int sleep_nano (long nsec);

double scale;
short bar_heights[NUM_BANDS];
static int sanalyzer_color = 1;

void sanalyzer_change_color (int foo)
{
    switch (sanalyzer_color) {
	case GREENRED:
	    sanalyzer_color = BLUEWHITE; break;
	case BLUEWHITE:
	    sanalyzer_color = BLUERED; break;
	case BLUERED:
	    sanalyzer_color = GREENRED; break;
    }
}

int sanalyzer_get_color (int j, int k)
{
    switch (sanalyzer_color) {
	case GREENRED:
	    return (gl_rgbcolor(j*k, 255-j*k, 0));
	case BLUEWHITE:
	    return (gl_rgbcolor(j*k, j*k, 255));
	case BLUERED:
	    return (gl_rgbcolor(j*k, 0, 255-j*k));
    }
}

void sanalyzer_init (void)
{
    pipe(pipesanalyzer);
//    fcntl(pipesanalyzer[0], F_SETFL, O_NONBLOCK | O_SYNC);
    fcntl(pipesanalyzer[1], F_SETFL, O_NONBLOCK | O_SYNC);
    pipe(pipeposbar);
    fcntl(pipeposbar[0], F_SETFL, O_NONBLOCK | O_SYNC);
    fcntl(pipeposbar[1], F_SETFL, O_NONBLOCK | O_SYNC);
}

void sanalyzer_uninit (void)
{
    close(pipesanalyzer[0]);
    close(pipesanalyzer[1]);
    close(pipeposbar[0]);
    close(pipeposbar[1]);
}

void sanalyzer_clear (void)
{
    if (!updtr_verbose)
        gl_fillbox(SA_X, SA_Y - SA_H, SA_W, SA_H, COL_SANALYZER_BACK);
}

void sanalyzer_draw (void)
{
    int i, j, c, k = 255/SA_H;
    int x = SA_X;

    sleep_nano((long)50000000);
    sanalyzer_clear();
    for (i=0; i<NUM_BANDS; i++) {
	for (j=bar_heights[i]; j>0; j--) {
	    c = sanalyzer_get_color(j, k);
    	    gl_fillbox(x , SA_Y - j, SA_W/NUM_BANDS - 2, 1, c);
	}
	for (j=0; j<SA_H; j+=5)
	    gl_fillbox(x, SA_Y-j, SA_W/NUM_BANDS, 2, COL_SANALYZER_BACK);
	x += SA_W/NUM_BANDS;
    }

}

static void calc_stereo_pcm(short dest[2][512], short src[2][512], int nch)
{
    memcpy(dest[0], src[0], 512 * sizeof(short));
    if(nch == 1)
	memcpy(dest[1], src[0], 512 * sizeof(short));
    else
	memcpy(dest[1], src[1], 512 * sizeof(short));
}

static void calc_mono_pcm(short dest[2][512], short src[2][512], int nch)
{
    int i;
    short *d, *sl, *sr;
	
    if (nch == 1)
	memcpy(dest[0], src[0], 512 * sizeof(short));
    else {
	d = dest[0];
	sl = src[0];
	sr = src[1];
	for (i = 0; i < 512; i++) {
	    *(d++) = (*(sl++) + *(sr++)) >> 1;
	}
    }
}

static void calc_freq (short *dest, short *src)
{
    static fft_state *state = NULL;
    float tmp_out[257];
    int i;

    if (!state)
	state = fft_init();

    fft_perform(src, tmp_out, state);

    for (i = 0; i < 256; i++)
	dest[i] = ((int) sqrt(tmp_out[i + 1])) >> 8;
}

static void calc_mono_freq (short dest[2][256], short src[2][512], int nch)
{
    int i;
    short *d, *sl, *sr, tmp[512];

    if (nch == 1)
	calc_freq(dest[0], src[0]);
    else {
	d = tmp;
	sl = src[0];
	sr = src[1];
	for (i = 0; i < 512; i++) {
	    *(d++) = (*(sl++) + *(sr++)) >> 1;
	}
	calc_freq(dest[0], tmp);
    }
}

static void calc_stereo_freq (short dest[2][256], short src[2][512], int nch)
{
    calc_freq(dest[0], src[0]);

    if (nch == 2)
	calc_freq(dest[1], src[1]);
    else
	memcpy(dest[1], dest[0], 256 * sizeof(short));
}

static void render_freq (short data[2][256])
{
    int i,c;
    int y;

    //16
//    int xscale[] = {0, 1, 2, 3, 5, 7, 10, 14, 20, 28, 40, 54, 74, 101, 137, 187, 255};
    //20
    int xscale[] = {0,1,2,3,4,5,6,7,8,11,15,20,27,36,47,62,82,107,141,184,255};
    //76
//    int xscale[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,61,66,71,76,81,87,93,100,107,114,122,131,140,150,161,172,184,255};

    for (i = 0; i < NUM_BANDS; i++) {
    	for (c = xscale[i], y = 0; c < xscale[i+1]; c++)
	    if (data[0][c] > y)
		y = data[0][c];

	y >>= 7;
	
	if (y != 0) {
	    y = (int)(log(y) * scale);
	    if (y > SA_H - 1)
		y = SA_H - 1;
	}
 
	if (y > bar_heights[i])
	    bar_heights[i] = y;
	else if (bar_heights[i] > 4)
	    bar_heights[i] -= 4;
	else
	    bar_heights[i] = 0;
    }
    
    sanalyzer_draw();
    return;			
}

void sanalyzer (short p_data[2][512], int chans)
{
    short pcm_data[2][512];
    short mono_freq[2][256];
    short stereo_freq[2][256];
    
    scale = (SA_H / log(256));
    
    if (chans == 1) {
	calc_mono_pcm(pcm_data, p_data, chans);
        calc_mono_freq(mono_freq, pcm_data, chans);
        render_freq(mono_freq);
    } else {
	calc_stereo_pcm(pcm_data, p_data, chans);
        calc_stereo_freq(stereo_freq, pcm_data, chans);
	render_freq(stereo_freq);
    }
}
