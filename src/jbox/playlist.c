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
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vgamouse.h>
#include <signal.h>
#include <linux/cdrom.h>

// Compare without regard to case.
#ifndef FNM_CASEFOLD
#	define FNM_CASEFOLD	 (1 << 4)
#endif

#include "../font.h"
#include "../kbd.h"
#include "../mouse.h"
#include "../utils.h"
#include "../main.h"
#include "playlist.h"
#include "updater.h"
#include "libcda/cda.h"

static int plpart = 1;
static int plshownfiles = 0;
static char plfiles[29][NAME_MAX];
int selected;
static int selectedy;

static int plpartcd = 1;
static int plshownfilescd = 0;
static char plfilescd[29][NAME_MAX];
static int selectedcd;
static int selectedycd;

int sortme (const void *first, const void *second);

void playlist_clear (void)
{
    gl_fillbox(292, 29, 263, 380, COL_PLAYLIST_BACK);
    gl_fillbox(552, 92, 48, 276, COL_PLAYLIST_BACK);
}

int playlist_getslash (int n, char *file, char *tmp)
{
    file = (char *)strrchr(file, '/');
    file++;
    sprintf(tmp, "%d. %s", n, file);
}

int playlist_show (void)
{
    int i, j, w;
    char file[PATH_MAX];
    char tmp[NAME_MAX];
    int y = 31;
    int total;
    
    total = totalfiles;

    if (total%29 == 0) {
        if (plpart > total/29) {
	    plpart--;
	    return;
	}
    } else {
	if (plpart > (total/29)+1) {
	    plpart--;
	    return;
	}
    }
    if (plpart<1) {
	plpart = 1;
	return;
    }
    
    playlist_clear();
    selected = 0;

    for (i=((29*(plpart-1))+1), j=0; i<((29*plpart)+1); i++, j++) {
	if (i>total)
	    break;
	playlist_i2c(playlist_fd, file, i);
	playlist_getslash(i, file, plfiles[j]);
	if (j>4 && j<26)
	    w = 307;
	else
	    w = 263;
	_drawtext(292, y, COL_PLAYLIST_TEXT, helvR08, plfiles[j], w, 13, FONTNOSCROLL, 1);
	y += 13;
    }
    plshownfiles = i-((29*(plpart-1))+1);
}

int playlist_sort (int fd, int d, int dd)
{
    char **files;
    char *file;
    int n = 0;
    char c;
    int i = 0, j = 0;
    
    lseek(fd, 0, SEEK_SET);
    
    while (read(fd, &c, 1) != 0)
	if (c == '\n')
	    n++;

    files = (char **) calloc(sizeof(char *), n);
    for (i=0; i<n; i++)
        files[i] = (char *) calloc(sizeof(char), PATH_MAX);
    file = (char *) calloc(1, PATH_MAX);

    lseek(fd, 0, SEEK_SET);
    
    for (j = 0; j < n; j++) {
	for (i= 0; read(fd, &c, 1) != 0; i++) {
	    if (c == '\n')
		break;
	    files[j][i] = c; 
	}
    }
    
    close(fd);    
    unlink(PLAYLIST_MP3);
    fd = playlist_open();
        
    qsort(files,n,sizeof(char *),sortme);

    for (j=0; j<n; j++) {
	if (d==1)
	    if (dd-1 == j)
		continue;
	sprintf(file, "%s\n", files[j]);
	write(fd, file, strlen(file));
    }

    for (i=0; i<n; i++)
	free(files[i]);
    free(files);
    free(file);
}

int playlist_open (void)
{
    int fd;
    fd = open(PLAYLIST_MP3, O_RDWR | O_CREAT, 00644);
    if (fd != -1)
	return fd;
    return 0;
}

int playlist_del_file (int fd, int i)
{
    if (i>0) {
        playlist_sort(fd, 1, i);
	totalfiles = playlist_total(playlist_fd);
	playlist_show();
    }
}

int playlist_add_file (int fd, char *buf)
{
    lseek(fd, 0, SEEK_END);
    write(fd, buf, strlen(buf));
    write(fd, "\n", 1);
}

int playlist_add_filea (int fd, char *match)
{
    DIR *dir;
    char dirname[NAME_MAX+1];
    struct dirent *dirent;
    char *file;
    char buf[PATH_MAX];
    struct stat statbuf;
   
    getcwd(dirname, sizeof(dirname));
    if ((dir = opendir(dirname)) == NULL)
	return 0;
   
    while ((dirent = readdir(dir)) != NULL) {
        file = (dirent->d_name);
        stat(file,&statbuf);
	if (S_ISREG(statbuf.st_mode)==1) { //it is a regular file
	    if (fnmatch(match, file, FNM_CASEFOLD) == 0) {
		sprintf(buf, "%s/%s", dirname, file);
		playlist_add_file(fd, buf);
	     }
	}
    }
    closedir(dir);
}

void dirwalk (char *dir, char *match)
{
    char name[NAME_MAX+1];
    struct dirent *dp;
    DIR *dfd;

    dfd = opendir(dir);
    
    while ((dp = readdir(dfd)) != NULL) {
	if (strcmp(dp->d_name, ".") == 0
	 || strcmp(dp->d_name, "..") == 0)
	    continue;
	sprintf(name, "%s/%s", dir, dp->d_name);
	playlist_add_dir(playlist_fd, name, match);
    }
    closedir(dfd);
}

int playlist_add_dir(int fd, char *dir, char *match)
{
    struct stat stbuf;
    char dirname[NAME_MAX+1];
    char buf[PATH_MAX];
    
    getcwd(dirname, sizeof(dirname));

    stat(dir, &stbuf);
    
    if (S_ISDIR(stbuf.st_mode) == 1)
	dirwalk(dir, match);
    if (S_ISREG(stbuf.st_mode) == 1)
	if (fnmatch(match, dir, FNM_CASEFOLD) == 0) {
    	    sprintf(buf, "%s/%s", dirname, dir);
	    playlist_add_file(fd, buf);
	}
}

/* int to char */
int playlist_i2c (int fd, char *tmp, int num)
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

int playlist_total (int fd)
{
    int i = 0;
    char c;

    lseek(fd, 0, SEEK_SET);
    while (read(fd, &c, 1) != 0)
        if (c=='\n')
	    i++;

    return i;
}

#define playlist_selecting_func(a)	if_mouse_pressed_over(292, y, 263, 13, a) {\
					    if (selected != 0) {\
						playlist_i2c(playlist_fd, file, selected);\
						playlist_getslash(selected, file, file1);\
						gl_fillbox(292, selectedy-2, ww, 13, COL_PLAYLIST_BACK);\
						_drawtext(292, selectedy, COL_PLAYLIST_TEXT, helvR08, file1, ww, 13, FONTNOSCROLL, 1);\
					    }\
					    gl_fillbox(292, y-2, w, 13, COL_PLAYLIST_TEXT);\
					    _drawtext(292, y, COL_PLAYLIST_BACK, helvR08, plfiles[i], w, 13, FONTNOSCROLL, 1);\
					    selected = i+1+(plpart-1)*29;\
					    selectedy = y;
void mp3_playlist_handler (int k)
{
    int i, w, ww, s;
    int y = 31;
    char file[PATH_MAX];
    char file1[NAME_MAX];
    struct popup rightbutton_popup;
    int popupi, popupy;
    int bhvr =  0;
    char btn_box[50*50*2];

    // scroll up
    if_mouse_pressed_over(579, 22, 46, 23, MOUSE_LEFTBUTTON)
	bhvr = 1;
    // scroll down
    if_mouse_pressed_over(579, 46, 46, 23, MOUSE_LEFTBUTTON)
	bhvr = 2;
    // insert
    if_mouse_pressed_over(604, 88, 36, 34, MOUSE_LEFTBUTTON)
	bhvr = 3;
    // del sellected
    if_mouse_pressed_over(604, 120, 36, 36, MOUSE_LEFTBUTTON)
	bhvr = 4;
    // del all
    if_mouse_pressed_over(604, 156, 36, 36, MOUSE_LEFTBUTTON)
	bhvr = 5; // ask
    if_mouse_pressed_over(604, 156, 36, 36, MOUSE_RIGHTBUTTON)
	bhvr = 51; // do not ask

    switch (k) {
	case RK_PAGE_UP :
	    bhvr = 1; break;
	case RK_PAGE_DOWN :
	    bhvr = 2; break;
	case RK_HOME :
	    plpart=1;
	    playlist_show();
	    break;
	case RK_INSERT :
	    bhvr = 3; break;
    }
    
    switch (bhvr) {
	case 1 : // scroll up
	    do_handler_case(579, 22, 46, 23, IMG_JBOX_BTN_SCRLU, btn_box, 0);
	    plpart--;
	    playlist_show();
	    break;
	case 2 : // scroll down
	    do_handler_case(579, 46, 46, 23, IMG_JBOX_BTN_SCRLD, btn_box, 0);
	    plpart++;
	    playlist_show();
	    break;
	case 3 : // insert
	    do_handler_case(604, 88, 36, 34, NULL, btn_box, 1);
	    if (updaterpid>0)
	        kill(updaterpid, SIG_UPDTR_VERBOSE);
	    updater_verbose_onoff(0);
	    sleep_nano((long)50000000);
	    dirbrowser("/", "*.mp3");
	    if (updaterpid>0)
		kill(updaterpid, SIG_UPDTR_VERBOSE);
	    updater_verbose_onoff(0);
	    totalfiles = playlist_total(playlist_fd);
	    plpart = 1;
	    playlist_show();
	    break;
	case 4 : // del sellected
	    if (selected <= 0)
		break;
	    do_handler_case(604, 120, 36, 36, NULL, btn_box, 1);
	    playlist_del_file(playlist_fd, selected);
	    break;
	case 5 : // del all
	    if (totalfiles <= 0) {
		totalfiles = 0; break;
	    }
	    do_handler_case(604, 156, 36, 36, NULL, btn_box, 1);
	    if (updaterpid>0)
	        kill(updaterpid, SIG_UPDTR_VERBOSE);
	    updater_verbose_onoff(0);
	    sleep_nano((long)50000000);
	    s = question_box(TEXT_QUEST, TEXT_DELLALL);
	    if (updaterpid>0)
		kill(updaterpid, SIG_UPDTR_VERBOSE);
	    updater_verbose_onoff(0);
	    if (s==0)
		break;
	case 51 :
	    if (totalfiles <= 0) {
		totalfiles = 0; break;
	    }
	    do_handler_case(604, 156, 36, 36, NULL, btn_box, 1);
	    close(playlist_fd);    
	    unlink(PLAYLIST_MP3);
	    playlist_clear();
	    playlist_fd = playlist_open();
	    mp3set0();
	    totalfiles = 0;
	    plshownfiles = 0;
	    selected = 0;
	    selectedy = 0;
	    playlist_show();
	    break;
    }
	
    for (i=0; i<plshownfiles; i++) {
	if (i>4 && i<26)
	    w = 307;
	else
	    w = 263;
	if (selected-1-(plpart-1)*29>4 && selected-1-(plpart-1)*29<26)
	    ww = 307;
	else
	    ww = 263;
	    
	if_mouse_clicked_over(292, y, 263, 13, MOUSE_LEFTBUTTON)
	    mp3playy(selected);

	playlist_selecting_func(MOUSE_LEFTBUTTON)
	    _mouse_waitbuttonrealese();
	}

	playlist_selecting_func(MOUSE_RIGHTBUTTON)
	    rightbutton_popup.i = 3;
	    sprintf(rightbutton_popup.item[0], TEXT_PLAY);
	    sprintf(rightbutton_popup.item[1], TEXT_ID);
	    sprintf(rightbutton_popup.item[2], TEXT_REMOVE);
	    if (i>23)
		popupy = mouse.y-(15+4+13*(rightbutton_popup.i));
	    else
		popupy = mouse.y;
	    popupi = draw_popup(mouse.x, popupy, TEXT_FUNC, &rightbutton_popup);
	    
	    if (popupi == 2)
		playlist_del_file(playlist_fd, selected);
	    if (popupi == 1) {
	        playlist_i2c(playlist_fd, file, selected);
		if (updaterpid>0)
	    	    kill(updaterpid, SIG_UPDTR_VERBOSE);
		updater_verbose_onoff(0);
		sleep_nano((long)50000000);
		mp3_printinfo(file);
		if (updaterpid>0)
		    kill(updaterpid, SIG_UPDTR_VERBOSE);
		updater_verbose_onoff(0);
	    }
	    if (popupi == 0)
		mp3playy(selected);
	    _mouse_waitbuttonrealese();
	}
	y+=13;
    }
}

void playlist_i2c_cd (char *name, int i)
{
    sprintf(name, "%d, CD Audio Track %02d", i, i);
}

int playlist_show_cd (void)
{
    int i, j, w;
    int y = 31;
    int total;
    
    total = totalfilescd;
    
    if (total == 0) {
	playlist_clear();
        selectedcd = 0;
	_drawtext(292, y, COL_PLAYLIST_TEXT, helvR08, TEXT_NOAUDIOCD, 263, 13, FONTNOSCROLL, 1);
	return;
    }

    if (total%29 == 0) {
	if (plpartcd > total/29) {
	    plpartcd--;
	    return;
	}
    } else {
        if (plpartcd > (total/29)+1) {
	    plpartcd--;
	    return;
	}
    }
    if (plpartcd<1) {
        plpartcd = 1;
        return;
    }
    
    playlist_clear();
    selectedcd = 0;

    for (i=((29*(plpartcd-1))+1), j=0; i<((29*plpartcd)+1); i++, j++) {
        if (i>total)
	    break;
	playlist_i2c_cd(plfilescd[j], i);
	if (j>4 && j<26)
	    w = 307;
	else
	    w = 263;
	_drawtext(292, y, COL_PLAYLIST_TEXT, helvR08, plfilescd[j], w, 13, FONTNOSCROLL, 1);
	y += 13;
    }
    plshownfilescd = i-((29*(plpartcd-1))+1);
}

#define playlist_selecting_funccd(a)	if_mouse_pressed_over(292, y, 263, 13, a) {\
					    if (selectedcd != 0) {\
						playlist_i2c_cd(file, selectedcd);\
						gl_fillbox(292, selectedycd-2, ww, 13, COL_PLAYLIST_BACK);\
						_drawtext(292, selectedycd, COL_PLAYLIST_TEXT, helvR08, file, ww, 13, FONTNOSCROLL, 1);\
					    }\
					    gl_fillbox(292, y-2, w, 13, COL_PLAYLIST_TEXT);\
					    _drawtext(292, y, COL_PLAYLIST_BACK, helvR08, plfilescd[i], w, 13, FONTNOSCROLL, 1);\
					    selectedcd = i+1+(plpartcd-1)*29;\
					    selectedycd = y;

void cd_playlist_handler (int k)
{
    int i, w, ww, bhvr = 0;
    int y = 31;
    char file[PATH_MAX];
    char file1[NAME_MAX];
    struct popup rightbutton_popup;
    struct popup insert_popup;
    int popupi, popupy;
    char btn_box[50*50*2];
    
    // scroll up
    if_mouse_pressed_over(579, 22, 46, 23, MOUSE_LEFTBUTTON)
	bhvr = 1;
    // scroll down
    if_mouse_pressed_over(579, 46, 46, 23, MOUSE_LEFTBUTTON)
	bhvr = 2;
    // change cd drive if possible
    if_mouse_pressed_over(604, 88, 36, 34, MOUSE_LEFTBUTTON)
	bhvr = 3;
        
    switch (k) {
	case RK_PAGE_UP :
	    bhvr = 1; break;
	case RK_PAGE_DOWN :
	    bhvr = 2; break;
	case RK_HOME :
	    plpartcd=1;
	    playlist_show_cd();
	    break;
	case RK_INSERT :
	    bhvr = 3; break;
    }
    switch (bhvr) {
	case 1 : // scroll up
	    do_handler_case(579, 22, 46, 23, IMG_JBOX_BTN_SCRLU, btn_box, 0);
	    plpartcd--;
	    playlist_show_cd();
	    break;
	case 2 : // scroll down
	    do_handler_case(579, 46, 46, 23, IMG_JBOX_BTN_SCRLD, btn_box, 0);
	    plpartcd++;
	    playlist_show_cd();
	    break;
	case 3 : // change cd drive if possible
	    insert_popup.i = 0;
	    if (access("/dev/cdroms/cdrom0", R_OK) == 0) {
	        sprintf(insert_popup.item[0], "/dev/cdroms/cdrom0");
		(insert_popup.i)++;
	    }
	    if (access("/dev/cdroms/cdrom1", R_OK) == 0) {
		sprintf(insert_popup.item[1], "/dev/cdroms/cdrom1");
		(insert_popup.i)++;
	    }
	    if (access("/dev/cdroms/cdrom2", R_OK) == 0) {
		sprintf(insert_popup.item[1], "/dev/cdroms/cdrom2");
		(insert_popup.i)++;
	    }
	    if (access("/dev/cdroms/cdrom3", R_OK) == 0) {
		sprintf(insert_popup.item[1], "/dev/cdroms/cdrom3");
		(insert_popup.i)++;
	    }
	    popupi = draw_popup(604-(_drawtext(0, 0, 0, helvR08, "/dev/cdroms/cdrom0", 150, 30, FONTSCROLL, 0))-15, 88, TEXT_CDDEVICE, &insert_popup);
	    if ((popupi >= 0) && (popupi <= insert_popup.i))
		cd_change(insert_popup.item[popupi]);
	    _mouse_waitbuttonrealese();
	    break;
    }
    for (i=0; i<plshownfilescd; i++) {
	if (i>4 && i<26)
	    w = 307;
	else
	    w = 263;
	if (selectedcd-1-(plpartcd-1)*29>4 && selectedcd-1-(plpartcd-1)*29<26)
	    ww = 307;
	else
	    ww = 263;
	    
	if_mouse_clicked_over(292, y, 263, 13, MOUSE_LEFTBUTTON)
	    cdplayy(selectedcd);

	playlist_selecting_funccd(MOUSE_LEFTBUTTON)}

	playlist_selecting_funccd(MOUSE_RIGHTBUTTON)
	    rightbutton_popup.i = 2;
	    sprintf(rightbutton_popup.item[0], TEXT_PLAY);
	    sprintf(rightbutton_popup.item[1], TEXT_CDDB);
	    if (i>23)
		popupy = mouse.y-(15+4+13*(rightbutton_popup.i));
	    else
		popupy = mouse.y;
	    popupi = draw_popup(mouse.x, popupy, TEXT_FUNC, &rightbutton_popup);
	    
	    if (popupi == 0)
		cdplayy(selectedcd);
	    _mouse_waitbuttonrealese();
	}
	y+=13;
    }
}
