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
#include "cddb.h"

struct cd_tracks {
    int first;
    int last;
};

struct msf {
    int minutes;
    int seconds;
    int frames;
};

struct cd_track {
    int track; // track number
    int nframes; // total frames
    int lba;	// start point (lba)
    struct msf msf;  // track lenght
    struct msf position; // playing position of track in MSF.
    struct msf cposition; // playing position of cd in MSF.
    int percentage; // playin position of track in %.
};

/*
 * return value;
 * success:	file describtor
 * error:	0
 */
int cd_open (char *device);

/*
 * return values for all;
 * success:	1
 * error:	0
 */
int cd_eject (int fd);

int cd_close (int fd);

/*
 * sets cdtracks->first
 *	cdtracks->last
 */
int cd_tracks (int fd, struct cd_tracks *cdtracks);

/*
 * see cdtrack struct
 * cdtrack->track must be set before calling this function
 */
int cd_track (int fd, struct cd_track *cdtrack);

/*
 * returns lba, not 1 || 0
 */
int cd_msf2lba (int min, int sec, int frm);

/*
 * sets msf struct
 */
int cd_lba2msf (int lba, struct msf *msf);

/*
 * see cdtrack struct
 * cdtrack->track must be set and
 * cd_track() function must be called before calling this function.
 *
 * sets cd_tack struct ( position and percentage ).
 */
int cd_play (int fd, int pos, struct cd_track *cdtrack);
