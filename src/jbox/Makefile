include ../Make.inc

jbox_obj = cd.o mp3.o jbox.o sanalyzer.o fft.o updater.o playlist.o

all : main

*.o : *.h

.c.o :

main : $(jbox_obj)
	@make -C libmp3
	@make -C libid3
	@make -C libcda
	@make -C games
