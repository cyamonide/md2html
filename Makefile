
CC = g++
CFLAGS  = -g -Wall
INCLUDES = -I./include

vpath %.h ./include
vpath %.cpp ./src

all: main.o helper.o inlineparse.o md2html.o
	$(CC) $(CFLAGS) $(INCLUDES) -o bin/md2html $^

main.o: main.cpp md2html.o constants.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

md2html.o: md2html.cpp md2html.h helper.o inlineparse.o constants.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

helper.o: helper.cpp helper.h md2html.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

inlineparse.o: inlineparse.cpp inlineparse.h helper.o constants.h
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean: 
	$(RM) bin/md2html *.o *~