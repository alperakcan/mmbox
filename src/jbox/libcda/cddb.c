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
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>

#include "cda.h"

unsigned int cddb_sum (int n)
{
    unsigned int ret;

    ret = 0;
    while (n > 0) {
      ret += (n % 10);
      n /= 10;
    }
    return ret;
}

unsigned long cd_cddbid (int fd)
{
    unsigned int i, t = 0, n = 0;
    struct cd_tracks ctracks;
    struct cdrom_tocentry tocentry;
    struct msf cdtoc[100];
    
    if (!cd_tracks(fd, &ctracks))
	return 0;
    
    for (i = 1; i <= ctracks.last; i++) {
        tocentry.cdte_track = i;
        tocentry.cdte_format = CDROM_MSF;
        ioctl(fd, CDROMREADTOCENTRY, &tocentry);
        cdtoc[i-1].minutes = tocentry.cdte_addr.msf.minute;
        cdtoc[i-1].seconds = tocentry.cdte_addr.msf.second;
    }
    tocentry.cdte_track = 0xAA;
    tocentry.cdte_format = CDROM_MSF;
    ioctl(fd, CDROMREADTOCENTRY, &tocentry);
    cdtoc[ctracks.last].minutes = tocentry.cdte_addr.msf.minute;
    cdtoc[ctracks.last].seconds = tocentry.cdte_addr.msf.second;

    for (i = 0; i < ctracks.last; i++)
      n = n + cddb_sum((cdtoc[i].minutes * 60) + cdtoc[i].seconds);

    t = ((cdtoc[ctracks.last].minutes * 60) + cdtoc[ctracks.last].seconds) - ((cdtoc[0].minutes * 60) + cdtoc[0].seconds);

    return ((n % 0xff) << 24 | t << 8 | ctracks.last);
}
