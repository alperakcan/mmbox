include ../../Make.inc

CFLAGS = -s -pipe -DLINUX -Os -mcpu=i386 -DLANG_TR -DLAYER1 -DLAYER2 -DLAYER3
#CFLAGS = -DI386_ASSEM -DREENTRANT -DPENTIUM_OPT -DREAL_IS_FLOAT -DLINUX -DREAD_MMAP -DOSS -Os -mcpu=i386 -finline-functions -fomit-frame-pointer -funroll-all-loops -ffast-math -DLAYER1 -DLAYER2 -DLAYER3 -pipe -s

libmp3_obj = common.o dct64_i386.o decode_i386.o interface.o layer1.o layer2.o layer3.o tabinit.o

all : main

*.o : *.h

.c.o :

main : $(libmp3_obj)
