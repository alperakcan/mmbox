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
#include <sys/ioctl.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <linux/cdrom.h>

#include "cda.h"
#include "../../audio.h"
#include "../../main.h"
#include "../sanalyzer.h"

#define FRAMETOREAD 4 /* DO NOT CHANGE 4 */

int cd_open (char *device)
{
    int fd;
    /*
     * linux/cdrom.h;
     *
     * Additionally, as of Linux 2.1.x, all Linux application programs 
     * should use the O_NONBLOCK option when opening a CD-ROM device 
     * for subsequent ioctl commands.  This allows for neat system errors 
     * like "No medium found" or "Wrong medium type" upon attempting to 
     * mount or play an empty slot, mount an audio disc, or play a data disc.
     * Generally, changing an application program to support O_NONBLOCK
     * is as easy as the following:
     *       -    drive = open("/dev/cdrom", O_RDONLY);
     *       +    drive = open("/dev/cdrom", O_RDONLY | O_NONBLOCK);
     * It is worth the small change.
     */
    fd = open(device, O_RDONLY | O_NONBLOCK);
    if (fd < 0)
	return 0;
    return fd;
}

int cd_eject (int fd)
{
    if(ioctl(fd, CDROMEJECT) != 0)
	return 0;
    return 1;
}

int cd_close (fd)
{
    if(ioctl(fd, CDROMCLOSETRAY) != 0)
	return 0;
    return 1;
} 

int cd_tracks (int fd, struct cd_tracks *cdtracks)
{
    struct cdrom_tochdr tochdr;
    if(ioctl(fd, CDROMREADTOCHDR, &tochdr) != 0)
	return 0;
    cdtracks->first = tochdr.cdth_trk0;
    cdtracks->last = tochdr.cdth_trk1;
    return 1;
}

int cd_msf2lba (int min, int sec, int frm)
{
    return ( (min*60*75) + (sec*75) + frm - CD_MSF_OFFSET);
}

int cd_lba2msf (int lba, struct msf *msf)
{
    msf->minutes = (lba + CD_MSF_OFFSET) / (60*75);
    msf->seconds = (lba + CD_MSF_OFFSET) / 75 - msf->minutes * 60;
    msf->frames = (lba + CD_MSF_OFFSET) - msf->seconds * 75 - msf->minutes * 60 * 75;
    return 1;
}

int cd_track (int fd, struct cd_track *cdtrack)
{
    int lba0, lba1;
    struct cdrom_tochdr tochdr;
    struct cdrom_tocentry tocentry;
    struct msf msf;
    
    if (ioctl(fd, CDROMREADTOCHDR, &tochdr) != 0)
	return 0;
    if (cdtrack->track <= tochdr.cdth_trk1) {
	tocentry.cdte_track = cdtrack->track;
	tocentry.cdte_format =  CDROM_LBA;
	if (ioctl(fd, CDROMREADTOCENTRY, &tocentry) != 0)
	    return 0;
	if (tocentry.cdte_ctrl != CDROM_DATA_TRACK) {
	    // this is an audio track
	    cdtrack->lba = tocentry.cdte_addr.lba;
	    // next tracks start point, it is the end point of our track ;)
	    // if that was the last one, get cd end.
	    tocentry.cdte_track = ((cdtrack->track < tochdr.cdth_trk1) ? (cdtrack->track) + 1 : CDROM_LEADOUT);
	    if (ioctl(fd, CDROMREADTOCENTRY, &tocentry) != 0)
		return 0;
	    // total frms of the track
	    cdtrack->nframes = tocentry.cdte_addr.lba - cdtrack->lba;
	    // total length
//	    cd_lba2msf(cdtrack->nframes, &msf);
//	    cdtrack->msf.minutes = msf.minutes;
//	    cdtrack->msf.seconds = msf.seconds;
//	    cdtrack->msf.frames = msf.frames;
	    cdtrack->msf.minutes = (cdtrack->nframes + CD_MSF_OFFSET) / (60*75);
	    cdtrack->msf.seconds = (cdtrack->nframes + CD_MSF_OFFSET) / 75 - cdtrack->msf.minutes * 60;
	    cdtrack->msf.frames = (cdtrack->nframes + CD_MSF_OFFSET) - cdtrack->msf.seconds * 75 - cdtrack->msf.minutes * 60 * 75;
	    return 1;
	} else 	// this is not an audio track
	    return 0;
    }
    return 0;
}

int cd_play (int fd, int pos, struct cd_track *cdtrack)
{
    int fdone;
    short p_data[2][512];
    struct cdrom_read_audio read_audio;
    unsigned char buf[CD_FRAMESIZE_RAW * FRAMETOREAD];
    
    struct cdrom_subchnl subchnl;
    subchnl.cdsc_format = CDROM_MSF;

    read_audio.addr_format = CDROM_LBA;
    read_audio.nframes = FRAMETOREAD;
    read_audio.buf = buf;
    read_audio.addr.lba = (cdtrack->lba + (cdtrack->nframes / 100)*pos);
    
    dsp_set(2, 44100);
    
    for (; ; read_audio.addr.lba += FRAMETOREAD) {
        if (ioctl(fd, CDROMREADAUDIO, &read_audio) != 0)
	    continue;
	if (write(dsp_fd, buf, CD_FRAMESIZE_RAW * FRAMETOREAD) <= 0)
	    continue;
	write(pipesanalyzer[1], buf, 1024);
	fdone = (read_audio.addr.lba - cdtrack->lba);
	write(pipeposbar[1], &fdone, 2*sizeof(int));
	
	// update where we are
//    	cdtrack->percentage = (100 * (read_audio.addr.lba - cdtrack->lba)) / cdtrack->nframes;
	// take subchnl data
	if (ioctl(fd, CDROMSUBCHNL, &subchnl) != 0)
	    continue;
	// tracks position
//        cdtrack->position.minutes = subchnl.cdsc_reladdr.msf.minute;
//        cdtrack->position.seconds = subchnl.cdsc_reladdr.msf.second;
//        cdtrack->position.frames = subchnl.cdsc_reladdr.msf.frame;
	// cds position
//        cdtrack->cposition.minutes = subchnl.cdsc_absaddr.msf.minute;
//        cdtrack->cposition.seconds = subchnl.cdsc_absaddr.msf.second;
//        cdtrack->cposition.frames = subchnl.cdsc_absaddr.msf.frame;
	// dont play next track ;)
	// next track will be grater than prev, huh ;)
	// what if track was the last one?! ( for(; ; ;) ? ;) )
	if (cdtrack->track < subchnl.cdsc_trk)
	    break;
    }
    // finished
//    cdtrack->percentage = 0;
//    cdtrack->position.minutes = 0;
//    cdtrack->position.seconds = 0;
//    cdtrack->position.frames = 0;
//    cdtrack->cposition.minutes = 0;
//    cdtrack->cposition.seconds = 0;
//    cdtrack->cposition.frames = 0;
}
