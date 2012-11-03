TARGET  := indicator-text
SRCS    := indicator-text.c

CC?=gcc
SHELL   = /bin/sh
CFLAGS = -O2 -Wall -Werror -ggdb $(shell pkg-config --cflags --libs gtk+-3.0 appindicator3-0.1)

all: ${TARGET}

${TARGET}: ${SRCS}
		$(CC) $< $(CFLAGS) -o $@

clean:
		rm -f *~ *.o ${TARGET}

distclean: clean
