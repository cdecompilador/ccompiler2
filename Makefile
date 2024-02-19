include config.mk

SRC = main.c buffer.c
OBJ = ${SRC:.c=.o}

all: ccomp

.c.o:
	${CC} -c ${CFLAGS} $<

ccomp: ${OBJ}
	${CC} ${CFLAGS} ${LDFLAGS} ${OBJ} -o $@

clean:
	rm ${OBJ} ccomp

.PHONY: all clean
