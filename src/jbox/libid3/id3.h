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
typedef struct id3tag_s {
	char	magic[3];
	char	songname[30];
	char	artist[30];
	char	album[30];
	char	year[4];
	char	note[28];
	unsigned char	null;
	unsigned char	track;
	unsigned char	style;
} id3tag_t;

/* 
 * returns;
 * 	success 1,
 *	error	0.
 */
int id3_readtag (int fd, id3tag_t *id3tag);

char *id3_findstyle(int styleid);
