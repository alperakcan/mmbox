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
#include <fcntl.h>

#include "rcfile.h"
#include "main_files.h"
#include "libirman/ir.h"

int get_line (int fd, char *tmp, int num)
{
    char c;
    int i=0, n=0;
    
    lseek(fd, 0, SEEK_SET);
    if (n<num) {
	while (n<(num-1)) {
	    if (read(fd, &c, 1) != 0) {
		if (c=='\n')
	    	    n++;
	    } else
		return 0;
	}
	i = 0;
	while (read(fd, &c, 1) != 0)
	    if (c!='\n')
		i++;
	    else
		break;
    }
    lseek(fd, -i-1, SEEK_CUR);
    read(fd, tmp, i);
    tmp[i] = '\0';
    return 1;
}

void rcfile_create (void)
{
    int fd;
    char buff[500];
    
    fd = open(RCFILE, O_RDWR | O_CREAT, 00644);
    sprintf(buff, "[JBOX]\n"
		  "DSP_DEVICE=/dev/sound/dsp\n"
		  "MIXER_DEVICE=/dev/sound/mixer\n"
		  "CD_DEVICE=/dev/cdroms/cdrom0\n"
		  "IRMAN_DEVICE=/dev/tts/0\n\n"
		  "[IRMAN CODES]\n"
		  "CODE_QUIT=000000000000\n"
		  "CODE_PLAY=000000000000\n"
		  "CODE_PAUSE=000000000000\n"
		  "CODE_STOP=000000000000\n"
		  "CODE_NEXT=000000000000\n"
		  "CODE_PREV=000000000000\n"
		  "CODE_SHUFFLE=000000000000\n"
		  "CODE_REPLAY=000000000000\n"
		  "CODE_JBOX=000000000000\n"
		  "CODE_JBOXMP3=000000000000\n"
		  "CODE_JBOXCD=000000000000\n"
		  "CODE_VOL_UP=000000000000\n"
		  "CODE_VOL_DOWN=000000000000\n");
    write(fd, buff, strlen(buff));
    close(fd);
}

void rcfile_load (void)
{
    int fd, i=1;
    char buff[400];

    fd = open(RCFILE, O_RDONLY);
    if (fd == -1) {
	rcfile_create();
	fd = open(RCFILE, O_RDONLY);
    }
    while (get_line(fd, buff, i) == 1) {
	if (strncmp(buff, "DSP_DEVICE=", 11) == 0)
	    sprintf(cfg.dsp_device, "%s", (buff+11));
	if (strncmp(buff, "MIXER_DEVICE=", 13) == 0)
	    sprintf(cfg.mixer_device, "%s", (buff+13));
	if (strncmp(buff, "CD_DEVICE=", 10) == 0)
	    sprintf(cfg.cd_device, "%s", (buff+10));
	if (strncmp(buff, "IRMAN_DEVICE=", 13) == 0)
	    sprintf(cfg.irman_device, "%s", (buff+13));
	if (strncmp(buff, "CODE_QUIT=", 10) == 0)
	    sprintf(ir_codes[RI_QUIT], (buff+10));
	if (strncmp(buff, "CODE_PLAY=", 10) == 0)
	    sprintf(ir_codes[RI_PLAY], (buff+10));
	if (strncmp(buff, "CODE_PAUSE=", 11) == 0)
	    sprintf(ir_codes[RI_PAUSE], (buff+11));
	if (strncmp(buff, "CODE_STOP=", 10) == 0)
	    sprintf(ir_codes[RI_STOP], (buff+10));
	if (strncmp(buff, "CODE_NEXT=", 10) == 0)
	    sprintf(ir_codes[RI_NEXT], (buff+10));
	if (strncmp(buff, "CODE_PREV=", 10) == 0)
	    sprintf(ir_codes[RI_PREV], (buff+10));
	if (strncmp(buff, "CODE_SHUFFLE=", 13) == 0)
	    sprintf(ir_codes[RI_SHUFFLE], (buff+13));
	if (strncmp(buff, "CODE_REPLAY=", 12) == 0)
	    sprintf(ir_codes[RI_REPLAY], (buff+12));
	if (strncmp(buff, "CODE_JBOX=", 10) == 0)
	    sprintf(ir_codes[RI_JBOX], (buff+10));
	if (strncmp(buff, "CODE_JBOXMP3=", 13) == 0)
	    sprintf(ir_codes[RI_JBOXMP3], (buff+13));
	if (strncmp(buff, "CODE_JBOXCD=", 12) == 0)
	    sprintf(ir_codes[RI_JBOXCD], (buff+12));
	if (strncmp(buff, "CODE_VOL_UP=", 12) == 0)
	    sprintf(ir_codes[RI_VOL_UP], (buff+12));
	if (strncmp(buff, "CODE_VOL_DOWN=", 14) == 0)
	    sprintf(ir_codes[RI_VOL_DOWN], (buff+14));
	i++;
    }
    close(fd);
}

void rcfile_save (void)
{
    int fd, i;
    char buff[500];
    char ir_c[IRCODES][13];
    
    unlink(RCFILE);
    
    for (i=0; i<IRCODES; i++)
	snprintf(ir_c[i], 13, ir_codes[i]);
	
    fd = open(RCFILE, O_RDWR | O_CREAT, 00644);
    sprintf(buff, "[JBOX]\n"
		  "DSP_DEVICE=/dev/sound/dsp\n"
		  "MIXER_DEVICE=/dev/sound/mixer\n"
		  "CD_DEVICE=%s\n"
		  "IRMAN_DEVICE=/dev/tts/0\n\n"
		  "[IRMAN CODES]\n"
		  "CODE_QUIT=%s\n"
		  "CODE_PLAY=%s\n"
		  "CODE_PAUSE=%s\n"
		  "CODE_STOP=%s\n"
		  "CODE_NEXT=%s\n"
		  "CODE_PREV=%s\n"
		  "CODE_SHUFFLE=%s\n"
		  "CODE_REPLAY=%s\n"
		  "CODE_JBOX=%s\n"
		  "CODE_JBOXMP3=%s\n"
		  "CODE_JBOXCD=%s\n"
		  "CODE_VOL_UP=%s\n"
		  "CODE_VOL_DOWN=%s\n",
		  cfg.cd_device,
		  ir_c[RI_QUIT],
		  ir_c[RI_PLAY],
		  ir_c[RI_PAUSE],
		  ir_c[RI_STOP],
		  ir_c[RI_NEXT],
		  ir_c[RI_PREV],
		  ir_c[RI_SHUFFLE],
		  ir_c[RI_REPLAY],
		  ir_c[RI_JBOX],
		  ir_c[RI_JBOXMP3],
		  ir_c[RI_JBOXCD],
		  ir_c[RI_VOL_UP],
		  ir_c[RI_VOL_DOWN]);
    write(fd, buff, strlen(buff));
    close(fd);
}
