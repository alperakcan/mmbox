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
int playlist_fd;
int totalfiles;
int totalfilescd;
// opens file return fd.
int playlist_open (void);
// writes buf(file name) to fd (file) ;)
int playlist_add_file (int fd, char *buf);
// writes all files in current dir (match) to the playlist file.
int playlist_add_filea (int fd, char *match);
// writes all files in current and subdirs of dir (match) to the playlist file.
int playlist_add_dir(int fd, char *dir, char *match);
// sorts playlist
// if d == 1 then delete num dd.
int playlist_sort (int fd, int d, int dd);
// return total files in plist
int playlist_total (int fd);
// ;)
int playlist_getslash (int n, char *file, char *tmp);
// int to char 
int playlist_i2c (int fd, char *tmp, int num);
// clear plist area
void playlist_clear (void);
// shows playlist (mp3)
int playlist_show (void);
// playlist, kbd handler
void mp3_playlist_handler (int k);
void cd_playlist_handler (int k);
