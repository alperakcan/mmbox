CC = gcc

CFLAGS = -s -pipe -DLINUX -Os -mcpu=i386 

# Language (Only define one of them)
#English
CFLAGS += -DLANG_EN
# Turkce
#CFLAGS += -DLANG_TR

# Debug
#CFLAGS += -DDEBUG

# Embedded (Make sure you have RamFS support in your kernel and busybox at /sbin)
#CLFAGS += -DINSMOD -DEMBEDDED

# DO NOT TOUCH BELOW

main_obj = main.o mouse.o font.o utils.o dirbrowser.o audio.o kbd.o clock.o readbmp.o rcfile.o
libirman_obj = libirman/irfunc.o libirman/irio.o libirman/ir.o
jbox_obj = jbox/jbox.o jbox/cd.o jbox/mp3.o jbox/sanalyzer.o jbox/fft.o jbox/updater.o jbox/playlist.o
libmp3_obj = jbox/libmp3/common.o jbox/libmp3/dct64_i386.o jbox/libmp3/decode_i386.o jbox/libmp3/interface.o jbox/libmp3/layer1.o jbox/libmp3/layer2.o jbox/libmp3/layer3.o jbox/libmp3/tabinit.o
libid3_obj = jbox/libid3/id3.o
libcda_obj = jbox/libcda/cda.o jbox/libcda/cddb.o
games_obj = jbox/games/tetris.o jbox/games/minesweeper.o
