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
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <vgagl.h>
#include <vgamouse.h>
#include <sys/mount.h>
#include <errno.h>

#include "libmp3/mpg123.h"
#include "libmp3/mpglib.h"
#include "libid3/id3.h"

#include "mp3.h"
#include "jbox.h"
#include "updater.h"
#include "playlist.h"
#include "sanalyzer.h"
#include "../audio.h"
#include "../font.h"
#include "../mouse.h"
#include "../utils.h"
#include "../main.h"

#define DECODE_SIZE 4096
#define NOT_SELECTED 0
#define IN_BUF_SIZE 16384
#define OUT_BUF_SIZE 8192

extern int selected;
static int cd_mounted = 0;

void mp3play (int pos);
void mp3plisthandler (int foo);

char buf[IN_BUF_SIZE];

struct mp3now {
    int bytes_done;
    int fd;
    int fsize;
};

struct mp3_info {
    int freq;
    int chans;
    int brate;
    int secs;
    int mpeg;
    int layer;
    int mode;
};

extern int tabsel_123[2][3][16];

static struct player mp3 = {0, 1, 0, 0, 0, 0, 0};

static char *modes[4] = { "Stereo", "Joint-Stereo", "Dual-Channel", "Single-Channel" };
static char *layers[4] = { "Unknown" , "I", "II", "III" };

/* get file size in bytes of the file */
int getfsize (int filedes)
{
    struct stat *status;
    int filesize;
    status = (struct stat *)malloc(sizeof(struct stat));
    fstat(filedes, status);
    filesize = status->st_size;
    free(status);
    return filesize;
}

// finds a suitable place to play ;)
int search_in_mp_file (struct mp3now *tmp, int bytes)
{
    int bbytes = 0;
    int i=0, j=0;
    if (tmp->fsize <= bytes+5)
	return MP3_ERR;
    lseek(tmp->fd, bytes, SEEK_SET);
    while (1) {
	if (tmp->fsize <= bytes+bbytes+5)
	    return MP3_ERR;
	read(tmp->fd, buf, 2);
	lseek(tmp->fd, -1, SEEK_CUR);
	bbytes++;
	tmp->bytes_done = bytes + bbytes;
	i += 2;
	/* ==255 >=224 */
	if ((buf[0] == '\xff') && (buf[1] >= '\xe0')) {
	    if (i-j-2 == 418 || i-j-2 == 417) {
		lseek(tmp->fd, -1, SEEK_CUR);
		return MP3_OK;
	    }
	    j=i-2;
	}
	i--;
    }
}

// sets structs ;)
int mp3_init (struct mp3now *mp3now, struct mp3_info *mp3_info)
{
    struct mpstr mp;
    char out[OUT_BUF_SIZE];
    int size, len;
    
    size = (DECODE_SIZE<bsize?DECODE_SIZE:bsize); 
    InitMP3(&mp);
    
    mp3now->fsize = getfsize(mp3now->fd);
    
    // experiences made me to do like this ;)
    if (search_in_mp_file(mp3now, 0) == MP3_ERR)
	lseek(mp3now->fd, 0, SEEK_SET);

    len = read(mp3now->fd, buf, IN_BUF_SIZE);
    
    if (decodeMP3(&mp, buf, len, out, OUT_BUF_SIZE, &size) != MP3_OK)
	return 0;
    
    mp3_info->freq = freqs[mp.fr.sampling_frequency];
    if ((mp3_info->brate = tabsel_123[mp.fr.lsf][mp.fr.lay-1][mp.fr.bitrate_index]) == 0)
	return 0;
    mp3_info->chans = mp.fr.stereo;
    mp3_info->secs = mp3now->fsize / (mp3_info->brate * 128);
    mp3_info->mpeg = mp.fr.mpeg25 ? 25 : (mp.fr.lsf ? 20 : 10);
    mp3_info->layer = mp.fr.lay;
    mp3_info->mode = mp.fr.mode;

    ExitMP3(&mp);
    return 1;
}

int mp3_printinfo (char *filename)
{
    char *buf;
    char *toptext;
    
    struct mp3now mp3now;
    struct mp3_info mp3_info;
    
    id3tag_t *id3tag;
    char style[30];
    char text0[] = {TEXT_ID3v1};
    char text1[200];
    char text2[150];
    char text3[150];
    
    mp3now.fd = open(filename, /*O_RDWR*/ O_RDONLY);
    if (mp3now.fd == -1) {
	info_box(TEXT_ERR, TEXT_NOFILE);
	return 0;
    }

    if (mp3_init(&mp3now, &mp3_info) == 0) {
	info_box(TEXT_ERR, TEXT_NOLAYER);
	return 0;
    }

    // ID3
    id3tag = (id3tag_t *)malloc(sizeof(id3tag_t));
    
    if (id3_readtag(mp3now.fd, id3tag)) {
	strcpy(style, id3_findstyle(id3tag->style));
	sprintf(text1,
		"%0.30s\n\n"
		"%0.30s\n\n"
		"%0.30s\n\n"
		"%0.4s\n\n"
		"%0.28s\n\n"
		"%0.1d\n\n"
		"%0.30s\n\n\0",
		id3tag->songname,
		id3tag->artist,
		id3tag->album,
		id3tag->year,
		id3tag->note,
		id3tag->track,
		style);
    } else
	text1[0] = '\0';

    sprintf(text2,
		"MPEG %s, Layer: %s, Mode: %s",
		(mp3_info.mpeg == 25) ? "2.5" : ((mp3_info.mpeg == 20) ? "2.0" : "1.0"),
		((mp3_info.layer<4) && (mp3_info.layer>=0)) ? layers[mp3_info.layer] : layers[0], ((mp3_info.mode<4) && (mp3_info.mode>=0)) ? modes[mp3_info.mode] : modes[0]);
    sprintf(text3,
		"Freq: %d, Bitrate: %d Kbits/s, Channels: %d, Time: %d secs\0",
		mp3_info.freq, mp3_info.brate, mp3_info.chans, mp3_info.secs);
    
    toptext = (char *) calloc(sizeof(char), (strlen(filename) + 16));
    playlist_getslash(selected, filename, filename);
    sprintf(toptext, TEXT_ID3v1TOP, filename);
    buf = (char *) calloc(2, (370*251));

    draw_box0(135, 114, 370, 251, toptext, buf);
    // left
    draw_button_area3(140, 134, 76, 173);
    _drawtext(143, 137, COL_BUTTON_TEXT, helvR08, text0, 70, 169, FONTNOSCROLL, 1);
    // right
    draw_button_area3(221, 134, 276, 173);
    _drawtext(224, 137, COL_BUTTON_TEXT, helvR08, text1, 270, 169, FONTNOSCROLL, 1);
    // buttom
    _drawtext(143, 314, COL_BUTTON_TEXT, helvR08, text2, 354, 13, FONTNOSCROLL, 1);
    _drawtext(143, 327, COL_BUTTON_TEXT, helvR08, text3, 354, 13, FONTNOSCROLL, 1);
    // OK button
    draw_button(295, 340, TEXT_OK, 0);
    while (1) {
	UPDATERS(1);
	if_mouse_pressed_over(295, 340, 50, 19, MOUSE_LEFTBUTTON) {
	    draw_button(295, 340, TEXT_OK, 1);
	    break;
	}
    }
    gl_putbox(135, 114, 370, 251, buf);
    close(mp3now.fd);
    free(toptext);
    free(buf);
    free(id3tag);
    return 1;
}

int execute_mp3(char *filename, int pos, struct mp3now mp3now, struct mp3_info mp3_info)
{
    struct mpstr mp;
        
    int size;
    char out[OUT_BUF_SIZE];
    int len, ret;
    int beginset;
    
    size = (DECODE_SIZE<bsize ? DECODE_SIZE : bsize);

    dsp_set(mp3_info.chans, mp3_info.freq);

    InitMP3(&mp);

    beginset = (mp3now.fsize / 100) * pos;
    mp3now.bytes_done = beginset;
	
    if (search_in_mp_file(&mp3now, beginset) == MP3_ERR)
	if (mp3_info.mpeg == 10)
	    return 0;
	else
	    lseek(mp3now.fd, 0, SEEK_SET);

    while (1) {
	len = read(mp3now.fd, buf, IN_BUF_SIZE);
    	mp3now.bytes_done += len;
	write(pipeposbar[1], &(mp3now.bytes_done), 2*sizeof(int));
		
	if (len<=0)
	    return 0;
	ret = decodeMP3(&mp, buf, len, out, OUT_BUF_SIZE, &size);
	if (mp3_info.freq != freqs[mp.fr.sampling_frequency])
	    if (dsp_set(mp.fr.stereo, freqs[mp.fr.sampling_frequency]) >= 0)
	        mp3_info.freq = freqs[mp.fr.sampling_frequency];
	while (ret==MP3_OK) {
	    write(dsp_fd, out, size);
	    write(pipesanalyzer[1], out, 1024*sizeof(short));
	    ret = decodeMP3(&mp, NULL, 0, out, OUT_BUF_SIZE, &size);
	}

	/* if something goes wrong try to play to the end of the file */
	if (ret==MP3_ERR) {
	    if (search_in_mp_file(&mp3now, (mp3now.bytes_done)+100) == MP3_ERR) {
		ExitMP3(&mp);
		return 0;
	    }
	    ExitMP3(&mp);
	    InitMP3(&mp);
	}
    }
    ExitMP3(&mp);
    return 1;
}

void mp3pause (void)
{
    boxpause(&(mp3.status), &(mp3.pid));
}

void mp3stop (int foo)
{
   boxstop(&(mp3.status), &(mp3.pid), &(mp3.upid));
}

int mp3playy (int num)
{
    mp3.num = num;
    mp3play(0);
}

void mp3play (int pos)
{
    short p_data[2][512];
    struct mp3now mp3now;
    struct mp3_info mp3_info;
    char filename[PATH_MAX];
    char updater_filename[NAME_MAX+60];
    char tmp[NAME_MAX];
    
    if ((pos>0) && (mp3.status == STOPPED))
	return;
    
    if (mp3.pid != 0)
	mp3stop(0);

    if (playlist_i2c(playlist_fd, filename, mp3.num) != 1)
	return;

    mp3now.fd = open(filename, O_RDONLY);
    if ((mp3now.fd == -1) || (mp3_init(&mp3now, &mp3_info) == 0)) {
	mp3plisthandler(0);
	return;
    }
    dsp_set(mp3_info.chans, mp3_info.freq);
    sanalyzer_init();
    
    mp3.pid = fork();
    if (mp3.pid == 0) {
	execute_mp3(filename, pos, mp3now, mp3_info);
	kill(getppid(), SIG_MP3PLIST);
	while (1);
    }
    if (mp3.pid>0) {
	mp3.upid = fork();
	if (mp3.upid == 0) {
	    playlist_getslash(mp3.num, filename, tmp);
	    sprintf(updater_filename, "%50s %s", "", tmp);
	    while (1) {
		read(pipesanalyzer[0], p_data, 1024*sizeof(short));
		read(pipeposbar[0], &(mp3now.bytes_done), 2*sizeof(int));
		updater(p_data, mp3_info.chans, updater_filename, mp3now.fsize, mp3now.bytes_done, mp3_info.brate);
		updater_printf_infos(modes[mp3_info.mode], mp3_info.brate, mp3_info.freq);
	    }
	}
	if (mp3.upid>0)
	    updaterpid = mp3.upid;
	mp3.status = PLAYING;
    }
}

int mp3next (void)
{
    if (totalfiles <= 0) {
	totalfiles = 0;
	mp3.num = 1;
	return 0;
    }
    if (boxnext(&(mp3.num), mp3.shuffle, mp3.replay, 1, totalfiles)) {
        if (mp3.status == PLAYING || mp3.status == PAUSED)
	    mp3play(0);
	else
	    return 1;
    } else
	return 0;
}

void mp3prev (void)
{
    if (totalfiles <= 0) {
	totalfiles = 0;
	mp3.num = 1;
	return;
    }
    if (boxprev(&(mp3.num), mp3.shuffle, mp3.replay, 1, totalfiles))
	if (mp3.status == PLAYING || mp3.status == PAUSED)
	    mp3play(0);
}

void mp3shuffle (void)
{
    on_off(&(mp3.shuffle));
    draw_shufrep(mp3.shuffle, mp3.replay);
}

void mp3replay (void)
{
    on_off(&(mp3.replay));
    draw_shufrep(mp3.shuffle, mp3.replay);
}

void mp3plisthandler (int foo)
{
    mp3stop(0);
    if (mp3next())
        mp3play(0);
}

void mp3boxuninit (void)
{
    mp3stop(0);
    close(playlist_fd);
    umount("/cdrom");
    cd_mounted = 0;
}

int mp3boxinit (void)
{
    playlist_clear();
    playlist_fd = playlist_open();
    totalfiles = playlist_total(playlist_fd);
    playlist_show();
    jbox.stop = mp3stop;
    jbox.play = mp3play;
    jbox.pause = mp3pause;
    jbox.next = mp3next;
    jbox.prev = mp3prev;
    jbox.shuffle = mp3shuffle;
    jbox.replay = mp3replay;
    jbox.playlist_handler = mp3_playlist_handler;
    jbox.uninit = mp3boxuninit;
    draw_shufrep(mp3.shuffle, mp3.replay);
    cd_mounted = 0;
    cd_mount();
}

void mp3set0 (void)
{
    mp3.num = 1;
}

void draw_mountstatus (void)
{
    if (!cd_mounted)
	gl_fillbox(611, 331, 6, 8, gl_rgbcolor(255, 0, 0));
    else
	gl_fillbox(611, 331, 6, 8, gl_rgbcolor(27, 170, 27));
}

void cd_mount (void)
{
    int ret;
    if (!cd_mounted) {
        ret = mount(cfg.cd_device, "/cdrom", "iso9660", MS_RDONLY, NULL);
	cd_mounted = 1;
    } else {
	umount("/cdrom");
	cd_mounted = 0;
    }
    if (ret == -1) {
	umount("/cdrom");
	cd_mounted = 0;
    }
    draw_mountstatus();
}

void cd_select_device (void)
{
    struct popup cd_popup;
    int popupi = 0;

    cd_popup.i = 0;    
    if (access("/dev/cdroms/cdrom0", R_OK) == 0) {
        sprintf(cd_popup.item[0], "/dev/cdroms/cdrom0");
	(cd_popup.i)++;
    }
    if (access("/dev/cdroms/cdrom1", R_OK) == 0) {
	sprintf(cd_popup.item[1], "/dev/cdroms/cdrom1");
	(cd_popup.i)++;
    }
    if (access("/dev/cdroms/cdrom2", R_OK) == 0) {
	sprintf(cd_popup.item[1], "/dev/cdroms/cdrom2");
	(cd_popup.i)++;
    }
    if (access("/dev/cdroms/cdrom3", R_OK) == 0) {
	sprintf(cd_popup.item[1], "/dev/cdroms/cdrom3");
	(cd_popup.i)++;
    }
    popupi = draw_popup(604-(_drawtext(0, 0, 0, helvR08, "/dev/cdroms/cdrom0", 150, 30, FONTSCROLL, 0))-15, 324, TEXT_CDDEVICE, &cd_popup);
    if ((popupi >= 0) && (popupi <= cd_popup.i)) {
	umount("/mnt/cdrom");
	cd_mounted = 0;
	sprintf(cfg.cd_device, "%s", cd_popup.item[popupi]);
	cd_mount();
	draw_mountstatus();
    }
    _mouse_waitbuttonrealese();
}
