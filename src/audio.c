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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/soundcard.h>
#include <vgagl.h>

#include "audio.h"
#include "main.h"

static int first_time = 1;
static char vol_box[29*33*2];

int dsp_init (void)
{
    dsp_fd = open(DSP_DEVICE, O_RDWR | O_SYNC);
    if (dsp_fd == -1)
	return 0;
    ioctl(dsp_fd, SNDCTL_DSP_RESET, 0);
    return 1;
}

int dsp_set (int chans, int srate)
{
    int depth = 16;
    int ret;
    
    ioctl(dsp_fd, SNDCTL_DSP_RESET, 0);
    /*srate*/
    ret = ioctl(dsp_fd, SOUND_PCM_WRITE_RATE, &srate);
    /*depth*/
    ioctl(dsp_fd, SOUND_PCM_WRITE_BITS, &depth);
    /*channels*/
    ioctl(dsp_fd, SOUND_PCM_WRITE_CHANNELS, &chans);
    /* bsize */
    ioctl(dsp_fd, SNDCTL_DSP_GETBLKSIZE, &bsize);
    ioctl(dsp_fd, SNDCTL_DSP_SYNC, 0);
    return ret;
}

void draw_vol (int vol)
{
    int l = (6*vol)/100;
    int c = gl_rgbcolor(0, 174, 255);
    if (first_time) {
	gl_getbox(607, 359, 29, 33, vol_box);
	first_time = 0;
    }
    gl_putbox(607, 359, 29, 33, vol_box);
    switch (l) {
	case 6:
	    gl_fillbox(608, 361, 2, 11, c);
	    gl_fillbox(610, 362, 1, 10, c);
	case 5:
	    gl_fillbox(614, 363, 1, 11, c);
	    gl_fillbox(615, 364, 1, 10, c);
	case 4:
	    gl_fillbox(619, 366, 1, 10, c);
	    gl_fillbox(620, 367, 1, 9, c);
	case 3:
	    gl_fillbox(624, 371, 1, 8, c);
	    gl_fillbox(625, 372, 1, 7, c);
	case 2:
	    gl_fillbox(629, 377, 1, 6, c);
	    gl_fillbox(630, 378, 1, 6, c);
	case 1:
	    gl_fillbox(633, 385, 1, 3, c);
	    gl_fillbox(634, 386, 1, 3, c);
	    break;
    }
}

int mixer_set (int vol)
{
    int fd;
    int i;
    int level;
        
    int devmask, stereodevs;
    
    if ((vol_current = vol) < 0) {
	vol_current = 0;
	return 0;
    }
    if ((vol_current = vol) > 100) {
	vol_current = 100;
	return 0;
    }
	    
    fd = open(MIXER_DEVICE, O_RDONLY);
    
    ioctl(fd, SOUND_MIXER_READ_DEVMASK, &devmask);
    ioctl(fd, SOUND_MIXER_READ_STEREODEVS, &stereodevs);
    
    for (i=0; i<SOUND_MIXER_NRDEVICES; i++) {
	level = (vol<<8) + vol;
	
	if ((1<<i) & devmask) {
	    if ((1<<i) & stereodevs)
		ioctl(fd, MIXER_WRITE(i), &level);
	    else
		ioctl(fd, MIXER_WRITE(i), &level);
	}
    }
    close(fd);
    draw_vol(vol_current);
    return 1;
}
