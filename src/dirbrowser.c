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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fnmatch.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vgagl.h>
#include <vgamouse.h>

#include "dirbrowser.h"
#include "utils.h"
#include "font.h"
#include "mouse.h"
#include "main.h"
#include "kbd.h"
#include "jbox/playlist.h"

// Compare without regard to case.
#ifndef FNM_CASEFOLD
#	define FNM_CASEFOLD	 (1 << 4)
#endif

#define HOWMANY		19 	/* Amount of lines that are going to print. */
#define SLIDEDIR	1
#define SLIDEFILE	2

// dir browser buttons
#define B_W 50
#define B_H 20
#define B_Y 365

#define B_OKx 509
#define B_OK(a) draw_button(B_OKx, B_Y, TEXT_OK, a)

#define B_ADDFILEx 454
#define B_ADDFILE(a) draw_button(B_ADDFILEx, B_Y, TEXT_ADDFILE, a)
#define B_ADDFILEALLx 399
#define B_ADDFILEALL(a) draw_button(B_ADDFILEALLx, B_Y, TEXT_ADDFILEALL, a)

#define B_MATCHtx 243
#define B_MATCHx B_MATCHtx+40
#define B_MATCH(a, b) draw_button(B_MATCHx, B_Y, b, a);\
		      _drawtext(B_MATCHtx, B_Y+4, COL_BUTTON_TEXT, helvR08, TEXT_MATCH, 50, 13, FONTNOSCROLL, 1);

#define B_ADDDIRx 80
#define B_ADDDIR(a) draw_button(B_ADDDIRx, B_Y, TEXT_ADDDIR, a)

// browser selector
typedef struct {
    char dir[NAME_MAX+1];
    char dirtmp[NAME_MAX+1];
    char file[NAME_MAX+1];
    char filetmp[NAME_MAX+1];
} SELECTOR;
static SELECTOR selector;

static int dirpartstodisplay = 0;  // number of directory parts to display
static int dirpart = 1;		   // the part that we will show
static int filepartstodisplay = 0; // number of file parts to display
static int filepart = 1;	   // the part that we will show

static char browserdir[HOWMANY][NAME_MAX+1];  // directories that are shown on browser
static char browserfile[HOWMANY][NAME_MAX+1]; // files that are shown on browser
static int browserdirs = 0;	// number of directories that are shown on browser
static int browserfiles = 0;    // number of files that are shown on browser

static char p[NAME_MAX+1];	// our lovely path.

static int slide = 0;

// qsort sorts dirs and files using this.
int sortme (const void *first, const void *second)
{
    return strcasecmp(*(char **) first, *(char **)second);
}

// routins for make_file_array & make_dir_array
int cc (int total, int part, int x, int w, char **buf, int df)
{
    int i = 0, j = 0;
    int countt = 0, count = 0;
    int ret = 0;
    
    countt = (total<(part*HOWMANY) ? total : (part*HOWMANY));
    if ((count%HOWMANY) == 0)
	count = (countt==total ? (part-1)*HOWMANY : countt-HOWMANY);
    else
	count = (countt==total ? (countt/HOWMANY)*HOWMANY : countt-HOWMANY);

    ret = countt - count;
    
    for (count; count < countt ; count++) {
	_drawtext(x, 113+i, COL_DIRBROWSER_TEXT, helvR08, buf[count], w, 13, FONTNOSCROLL, 1);
	i += 13;
	if (df)
	    strcpy(browserfile[j], buf[count]);
	else
	    strcpy(browserdir[j], buf[count]);
	j++;
    }
    return ret;
}

// writes file names to given matrix
// if totalfile == -1 than just calculates.
int make_file_array (const int totalfile, char **filename, char *match)
{
    DIR *dir;
    char dirname[NAME_MAX+1];
    struct dirent *dirent;
    char *file;
    struct stat statbuf;
    int count = 0;
   
    getcwd(dirname, sizeof(dirname));
    if ((dir = opendir(dirname)) == NULL)
	return 0;
   
    while ((dirent = readdir(dir)) != NULL) {
        file = (dirent->d_name);
        stat(file,&statbuf);
	if (S_ISREG(statbuf.st_mode)==1) { //it is a regular file
	    if (fnmatch(match, file, FNM_CASEFOLD) == 0) {
		if(totalfile != -1)
		    strcpy(filename[count],file);
	        count++;
	     }
	}
    }
    closedir(dir);
    if (totalfile == -1)
	return count;

    qsort(filename,totalfile,sizeof(char *),sortme);
    // screen printing
    browserfiles = cc(totalfile, filepart, 248, 296, filename, 1);
}

// writes directory names to given matrix
// if totaldir == -1 than just calculates.
int make_dir_array (const int totaldir, char **dir_name)
{
    DIR *dir;
    char dirname[NAME_MAX+1];
    struct dirent *dirent;
    char *file;
    struct stat statbuf;
    int count = 0;
    
    getcwd(dirname, sizeof(dirname));
    if ((dir = opendir(dirname)) == NULL)
	return 0;
    
    while ((dirent = readdir(dir)) != NULL) {
	file = (dirent->d_name);
	if (strcmp(file, ".") == 0 /*|| strcmp(file, "..") == 0*/)
	    continue;
	stat(file, &statbuf);
	if(S_ISDIR(statbuf.st_mode) == 1) { //it is a dir
	    if(totaldir != -1)
	        strcpy(dir_name[count], file);
	    count++;
	}
    }
    closedir(dir);
    if (totaldir == -1)
	return count;

    qsort(dir_name, totaldir, sizeof(char *), sortme);
    // screen printing
    browserdirs = cc(totaldir, dirpart, 85, 140, dir_name, 0);
}

void get_dir_file (char *match)
{
    int totalfile;
    int totaldir;
    int i;
    char **files;
    char **dirs;
    char statustext[50];
    char tmp[NAME_MAX+1];
    
    // prints working directory to screen
    getcwd(tmp, sizeof(tmp));
    gl_fillbox(80, 95, 480, 14, COL_BUTTON_3);
    _drawtext(80, 95, COL_DIRBROWSER_TEXT, helvR08, tmp, 480, 13, FONTNOSCROLL, 1);
    
    totalfile = make_file_array(-1, NULL, match);
    totaldir = make_dir_array(-1, NULL);
    
    dirpartstodisplay = (totaldir / HOWMANY);
    if ((totaldir % HOWMANY) != 0)
	dirpartstodisplay++;
    filepartstodisplay = (totalfile / HOWMANY);
    if (totalfile % HOWMANY != 0)
	filepartstodisplay++;

    files = (char **) calloc(sizeof(char *), totalfile);
    for (i=0; i<totalfile; i++)
        files[i] = (char *) calloc(sizeof(char), NAME_MAX+1);
    dirs = (char **) calloc(sizeof(char *), totaldir);
    for (i=0; i<totaldir; i++)
        dirs[i] = (char *) calloc(sizeof(char), NAME_MAX+1);

    if (slide == SLIDEDIR || slide == 0) {
	// clear dirs and files area
	// not to effect slector
        gl_fillbox(81, 111, 156, 248, COL_BUTTON_3);
	sprintf(selector.dir, "NOTHINGSELECTED");
	make_dir_array(totaldir, dirs);
    }
    if (slide == SLIDEFILE || slide == 0) {
	// clear dirs and files area
	// not to effect slector
        gl_fillbox(244, 111, 314, 248, COL_BUTTON_3);
	sprintf(selector.file, "NOTHINGSELECTED");
	make_file_array(totalfile, files, match);
    }
    
    // dirs slider
    if (HOWMANY<totaldir)
        draw_slider(225, 113, 243, dirpartstodisplay, dirpart);
    // files slider
    if (HOWMANY<totalfile)
        draw_slider(546, 113, 243, filepartstodisplay, filepart);
    
    gl_fillbox(80, 391, 480, 12, COL_BUTTON_3);
    sprintf(statustext, TEXT_DBSTATUS, totaldir, totalfile, match);
    _drawtext(80, 391, COL_DIRBROWSER_TEXT, helvR08, statustext, 480, 13, FONTNOSCROLL, 1);
    
    // clean up
    for (i=0; i<totaldir; i++)
	free(dirs[i]);
    for (i=0; i<totalfile; i++)
	free(files[i]);
    free(files);
    free(dirs);
}

void draw_dirbrowser (char *buf, char *match)
{
    // dir browser area
    draw_box0(75, 75, 490, 330, TEXT_DBTOP, buf);
    // dirs area
    draw_button_area3(80, 110, 158, 249);
    // files area
    draw_button_area3(243, 110, 316, 249);
    // status line
    gl_line(75, 389, 562, 389, COL_BUTTON_4);
    gl_line(75, 390, 562, 390, COL_BUTTON_0);
    // buttons
    B_OK(0);
    B_ADDFILE(0);
    B_ADDFILEALL(0);
    B_MATCH(0, match);
    B_ADDDIR(0);
}

// routins for dir browser
// selecting.
int bs (int x, int w, int i, int j, char *a, char *b, char *c)
{
    if_mouse_pressed_over(x, 113+13*i, w, 13, MOUSE_LEFTBUTTON) {
	_mouse_waitbuttonrealese();
	if (!strcmp(a, b)) {
	    gl_fillbox(x-2, 112+13*j, w+5, 13, COL_BUTTON_3);
	    _drawtext(x, 113+13*j, COL_DIRBROWSER_TEXT, helvR08, b, w+5, 13, FONTNOSCROLL, 1);
	}
	strcpy(a, c);
	gl_fillbox(x-2, 112+13*i, w+5, 13, COL_DIRBROWSER_SELECTED);
	_drawtext(x, 113+13*i, COL_BUTTON_3, helvR08, a, w+5, 13, FONTNOSCROLL, 1);
	j = i;
	strcpy(b, a);
    }
    return j;
}
// sliding
void ud (int x, int y, int *p, int ps, char *m, int s, int t)
{
    if_mouse_pressed_over(x, y, 10, 20, MOUSE_LEFTBUTTON) {
        _mouse_waitbuttonrealese();
        if ((*p > 1) && (t < 0))
	    (*p)--;
        if ((*p < ps) && (t > 0))
	    (*p)++;
        slide = s;
        get_dir_file(m);
    }
}

void dirbrowser (char *path, char *match)
{
    int i=0, dirj=0, filej=0;
    char dirtmp[NAME_MAX+1];
    char d[NAME_MAX+1];
    char *buf;
    struct popup match_popup;
    int matchi;
    char file[PATH_MAX];
    
    buf = calloc(2, 490*330);
    draw_dirbrowser(buf, match);

    getcwd(dirtmp, sizeof(dirtmp));
    strcpy(p, path);
    chdir(p);

    get_dir_file(match);

    while (1) {
	UPDATERS(1);
	// dir slider
	//up
	ud(225, 113, &dirpart, dirpartstodisplay, match, SLIDEDIR, -1);
	// down
	ud(225, 336, &dirpart, dirpartstodisplay, match, SLIDEDIR, 1);
	// dir selector
	for (i=0; i<browserdirs; i++) {
	    // to access dir
	    if_mouse_clicked_over(85, 113+13*i, 135, 13, MOUSE_LEFTBUTTON) {
		_mouse_waitbuttonrealese();
		dirpart = 1;
		filepart = 1;
		strcpy(p, browserdir[0+i]);
		chdir(p);
		slide = 0;
		get_dir_file(match);
	    }
	    // to select dir
	    dirj = bs(85, 135, i, dirj, selector.dir, selector.dirtmp, browserdir[0+i]);
	}
	// file slider
	// up
	ud(546, 113, &filepart, filepartstodisplay, match, SLIDEFILE, -1);
	// down
	ud(546, 336, &filepart, filepartstodisplay, match, SLIDEFILE, 1);
	// file selector
	for (i=0; i<browserfiles; i++)
	    // to select file
	    filej = bs(248, 293, i, filej, selector.file, selector.filetmp, browserfile[0+i]);
	
	// buttons
	if (key == RK_ESC)
	    goto QUIT_DIRBROWSE;
	if_mouse_pressed_over(B_OKx, B_Y, B_W, B_H, MOUSE_LEFTBUTTON) {
	    B_OK(1);
QUIT_DIRBROWSE:
	    playlist_sort(playlist_fd, 0, 0);
	    break;
	}
	if_mouse_pressed_over(B_ADDFILEx, B_Y, B_W, B_H, MOUSE_LEFTBUTTON) {
	    B_ADDFILE(1);
	    if ((strcmp(selector.file, "NOTHINGSELECTED")) != 0) {
		sprintf(file, "%s/%s", getcwd(d, sizeof(d)), selector.file);
		playlist_add_file(playlist_fd, file);
	    }
	    B_ADDFILE(0);
	}
	if_mouse_pressed_over(B_ADDFILEALLx, B_Y, B_W, B_H, MOUSE_LEFTBUTTON) {
	    B_ADDFILEALL(1);
	    playlist_add_filea(playlist_fd, match);
	    B_ADDFILEALL(0);
	}
	if_mouse_pressed_over(B_MATCHx, B_Y, B_W, B_H, MOUSE_LEFTBUTTON) {
	    B_MATCH(1, match);
	    
	    match_popup.i = 3;
//	    sprintf(match_popup.item[0], "*");
	    sprintf(match_popup.item[0], "*.mp3");
	    sprintf(match_popup.item[1], "*.mp2");
	    sprintf(match_popup.item[2], "*.mp1");
	    matchi = draw_popup(B_MATCHx, B_Y-(15+4+13*(match_popup.i)), TEXT_MATCH, &match_popup);
	    if ((matchi>=0) && (matchi<match_popup.i))
	        match = match_popup.item[matchi];
	    
	    dirpart = 1;
	    filepart = 1;
	    slide = 0;
	    get_dir_file(match);

	    B_MATCH(0, match);
	}
	if_mouse_pressed_over(B_ADDDIRx, B_Y, B_W, B_H, MOUSE_LEFTBUTTON) {
	    B_ADDDIR(1);
	    if ((strcmp(selector.dir, "..")) == 0) {
		chdir("..");
		playlist_add_dir(playlist_fd, p, match);
		chdir(p);
	    } else
	    if ((strcmp(selector.dir, "NOTHINGSELECTED")) != 0) {
		playlist_add_dir(playlist_fd, selector.dir, match);
	    }
	    B_ADDDIR(0);
	}
    }
    // clean up
    gl_putbox(75, 75, 490, 330, buf);
    free(buf);
    chdir(dirtmp);
}
