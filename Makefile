
CC = g++
CFLAGS  = -g -Wall
INCLUDES = -I./include

vpath %.h ./include
vpath %.cpp ./src

# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)
# you can name this target entry anything, but "default" or "all"
# are the most commonly used names by convention
#
default: md2html

# To create the executable file count we need the object files
# countwords.o, counter.o, and scanner.o:
#
md2html: md2html.o helper.o blockparse.o inlineparse.o
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o bin/md2html

md2html.o: md2html.cpp md2html.h state.h \
constants.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# To create the object file countwords.o, we need the source
# files countwords.c, scanner.h, and counter.h:
#
helper.o: helper.cpp helper.h state.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# To create the object file counter.o, we need the source files
# counter.c and counter.h:
#
blockparse.o: blockparse.cpp blockparse.h \
state.h constants.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# To create the object file scanner.o, we need the source files
# scanner.c and scanner.h:
#
inlineparse.o: inlineparse.cpp inlineparse.h \
constants.h helper.o
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
#
clean: 
	$(RM) bin/md2html *.o *~