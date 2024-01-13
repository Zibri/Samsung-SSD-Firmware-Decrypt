OBJS	= aes256.o sdec.o
SOURCE	= aes256.c sdec.c
HEADER	= aes256.h
OUT	= sdec
CC	= gcc
STRIP	= $(shell dirname `which $(CC)`)/strip
FLAGS	= -Os -g -c -Wall
LFLAGS	=

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)
ifeq ($(OS),Windows_NT)
	$(STRIP) -s $(OUT).exe
else
	$(STRIP) -s $(OUT)
endif
	rm *.o

aes256.o: aes256.c
	$(CC) $(FLAGS) aes256.c

sdec.o: sdec.c
	$(CC) $(FLAGS) sdec.c


clean:
ifeq ($(OS),Windows_NT)
	rm -f $(OBJS) $(OUT).exe
else
	rm -f $(OBJS) $(OUT)
endif

