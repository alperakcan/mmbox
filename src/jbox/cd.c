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
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <linux/cdrom.h>

#include "jbox.h"
#include "cd.h"
#include "../utils.h"
#include "../main.h"
#include "libcda/cda.h"
#include "sanalyzer.h"
#include "updater.h"
#include "playlist.h"

int got_info;
//char tray_box[8*8*2];
char garbage_box[38*(197-119)*2];
static struct player cd = {0, 1, 0, 0, 0, 0, 0};

void cdplay (int pos);
void draw_drivestatus (void);

int cdinfo (void)
{
    int discstatus, drivestatus;
    struct cd_tracks cdtracks;
    struct cd_track cdtrack;
    
    cd.fd = cd_open(cfg.cd_device);

    drivestatus = ioctl(cd.fd, CDROM_DRIVE_STATUS);
    if ((drivestatus == CDS_TRAY_OPEN) || (drivestatus == CDS_NO_DISC)) {
    	cd_close(cd.fd);
	drivestatus = ioctl(cd.fd, CDROM_DRIVE_STATUS);
    }

    discstatus = ioctl(cd.fd, CDROM_DISC_STATUS);
    if (discstatus != CDS_AUDIO) {
	totalfilescd = 0;
	goto PLAYLIST_SHOW_CD;
    }

    if (ioctl(cd.fd, CDROM_MEDIA_CHANGED))
	cd.num = 1;

    cd_tracks(cd.fd, &cdtracks);
    totalfilescd = cdtracks.last;
PLAYLIST_SHOW_CD:
    playlist_show_cd();
    draw_drivestatus();
}

void cdpause (void)
{
    boxpause(&(cd.status), &(cd.pid));
}

void cdstop (int foo)
{
    boxstop(&(cd.status), &(cd.pid), &(cd.upid));
    ioctl(cd.fd, CDROM_LOCKDOOR, 0);
}

int cdplayy (int num)
{
    cd.num = num;
    cdplay(0);
}

void cdplay (int pos)
{
    short p_data[2][512];
    struct cd_track cdtrack;
    char updater_filename[NAME_MAX+60];
    char tmp[NAME_MAX];
    int frames_done = 0;
    int drivestatus;
    
    drivestatus = ioctl(cd.fd, CDROM_DRIVE_STATUS);
    if ((drivestatus == CDS_TRAY_OPEN) || (drivestatus == CDS_NO_DISC) || (got_info == 0))
	cdinfo();

    if (((pos>0) && (cd.status == STOPPED)) || (totalfilescd == 0))
	return;
    
    if (cd.pid != 0)
	cdstop(0);

    cdtrack.track = cd.num;
    if (!cd_track(cd.fd, &cdtrack))
	return;

    dsp_set(2, 44100);
    sanalyzer_init();
    
    cd.pid = fork();
    if (cd.pid == 0) {
        cd_play(cd.fd, pos, &cdtrack);
        // routins for fork ;)
	kill(getppid(), SIG_CDPLIST);
	while (1);
    }

    if (cd.pid>0) {
	cd.upid = fork();
	if (cd.upid == 0) {
	    playlist_i2c_cd(tmp, cd.num);
	    sprintf(updater_filename, "%50s %s", "", tmp);
	    updater_printf_infos("Joint-Stereo", -1, 44100);
	    while (1) {
		read(pipesanalyzer[0], p_data, 1024);
		read(pipeposbar[0], &frames_done, 2*sizeof(int));
		updater(p_data, 2, updater_filename, cdtrack.nframes, frames_done, -1);
	    }
	}
	if (cd.upid>0)
	    updaterpid = cd.upid;
	cd.status = PLAYING;
	ioctl(cd.fd, CDROM_LOCKDOOR, 1);
    }
}

int cdnext (void)
{
    struct cd_tracks cdtracks;
    cd_tracks(cd.fd, &cdtracks);

    if (totalfilescd <= 0) {
	totalfilescd = 0;
	cd.num = 1;
	return 0;
    }
    
    if (boxnext(&(cd.num), cd.shuffle, cd.replay, cdtracks.first, cdtracks.last)) {
        if (cd.status == PLAYING || cd.status == PAUSED)
	    cdplay(0);
	else
	    return 1;
    } else
	return 0;
}

void cdprev (void)
{
    struct cd_tracks cdtracks;
    cd_tracks(cd.fd, &cdtracks);
    
    if (totalfilescd <= 0) {
	totalfilescd = 0;
	cd.num = 1;
	return;
    }

    if (boxprev(&(cd.num), cd.shuffle, cd.replay, cdtracks.first, cdtracks.last))
        if (cd.status == PLAYING || cd.status == PAUSED)
	    cdplay(0);
}

void cdshuffle (void)
{
    on_off(&(cd.shuffle));
    draw_shufrep(cd.shuffle, cd.replay);
}

void cdreplay (void)
{
    on_off(&(cd.replay));
    draw_shufrep(cd.shuffle, cd.replay);
}

void cdplisthandler (int foo)
{
    cdstop(0);
    if (cdnext())
        cdplay(0);
}

void cdboxuninit (void)
{
    cdstop(0);
    if (cd.fd > 0)
	close(cd.fd);
//    gl_putbox(611, 331, 8, 8, tray_box);
    gl_putbox(602, 119, 38, 197-119, garbage_box);
}

void draw_drivestatus (void)
{
    int drivestatus, discstatus;
    
    drivestatus = ioctl(cd.fd, CDROM_DRIVE_STATUS);
    discstatus = ioctl(cd.fd, CDROM_DISC_STATUS);
    if ((drivestatus == CDS_TRAY_OPEN) || (drivestatus == CDS_NO_DISC) || (discstatus != CDS_AUDIO)) {
	gl_fillbox(611, 331, 6, 8, gl_rgbcolor(255, 0, 0));
	got_info = 0;
    } else {
	gl_fillbox(611, 331, 6, 8, gl_rgbcolor(27, 170, 27));
	got_info = 1;
    }
}

int cdboxinit (void)
{
    int i, drivestatus;
    char clean_box[38*1*2];
    got_info = 1;
    dsp_set(2, 44100);
    playlist_clear();
//    sprintf(cfg.cd_device, "%s", CD_DEVICE);
    drivestatus = ioctl(cd.fd, CDROM_DRIVE_STATUS);
    drivestatus = ioctl(cd.fd, CDROM_DRIVE_STATUS);
    if ((drivestatus == CDS_TRAY_OPEN) || (drivestatus == CDS_NO_DISC))
	cdinfo();
//    gl_getbox(611, 331, 8, 8, tray_box);
    gl_getbox(602, 119, 38, 197-119, garbage_box);
    gl_getbox(602, 197, 38, 1, clean_box);
    for (i=119; i<198; i++)
	gl_putbox(602, i, 38, 1, clean_box);
    cdinfo();
    jbox.stop = cdstop;
    jbox.play = cdplay;
    jbox.pause = cdpause;
    jbox.next = cdnext;
    jbox.prev = cdprev;
    jbox.shuffle = cdshuffle;
    jbox.replay = cdreplay;
    jbox.playlist_handler = cd_playlist_handler;
    jbox.uninit = cdboxuninit;
    draw_shufrep(cd.shuffle, cd.replay);
}

void cdset0 (void)
{
    cd.num = 1;
}

void cd_change (char *dev)
{
    close(cd.fd);
    sprintf(cfg.cd_device, "%s", dev);
    cdinfo();
}
