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
// path, path name
void dirbrowser (char *path, char *match);

// x 75 y 75 w 490 h 330, 19 dirs, files fits
void draw_dirbrowser (char *buf, char *match);

// match, file type => "*.?"
void get_dir_file (char *match);

// read all file(s) name from current dir, sort it in nice order
int make_file_array (const int totalfile, char **filename, char *match);

// read all dir(s) name from current dir, sort it in nice order
int make_dir_array (const int totaldir, char **dir_name);

// for qsort - using strcmp 
int sortme (const void *first, const void *second);
