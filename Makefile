
CC = g++
CFLAGS  = -g -Wall
INCLUDES = -I./include

vpath %.h ./include
vpath %.cpp ./src

all: main.o buffer.o helper.o blockparse.o inlineparse.o
	$(CC) $(CFLAGS) $(INCLUDES) -o bin/md2html $^

main.o: main.cpp buffer.o state.h constants.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

buffer.o: buffer.cpp buffer.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

helper.o: helper.cpp helper.h state.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

blockparse.o: blockparse.cpp blockparse.h state.h constants.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

inlineparse.o: inlineparse.cpp inlineparse.h constants.h helper.o
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean: 
	$(RM) bin/md2html *.o *~