include Make.inc

TARGET = ../tetromino

SVGALIB_LIBS = -lvgagl -lvga

#STATIC_BUILD = -lm -static

OTHERLIBS =

all : main

main : $(main_obj)
	@make -C libirman
	@make -C jbox
	$(CC) -o $(TARGET) $(main_obj) $(libmp3_obj) $(libid3_obj) $(libcda_obj) $(jbox_obj) $(libirman_obj) $(games_obj) $(SVGALIB_LIBS) $(OTHERLIBS) $(STATIC_BUILD)
	@strip $(TARGET)
	@strip -x $(TARGET)
	@strip -R .note $(TARGET)
	@strip -R .comment $(TARGET)

main.o : main.h mouse.h
font.o : font.h fonts/fontbmps.h

mouse.o : mouse.h
utils.o : utils.h
dirbrowser.o : dirbrowser.h
audio.o : audio.h
kbd.o : kbd.h
clock.o : clock.h

clean : 
	@rm -f $(TARGET) $(main_obj) $(libmp3_obj) $(libid3_obj) $(libcda_obj) $(jbox_obj) $(libirman_obj) $(games_obj)
