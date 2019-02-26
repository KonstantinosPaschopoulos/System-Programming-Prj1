OBJS 	= bitcoin.o bitcoin_functions.o
SOURCE	= bitcoin.c bitcoin_functions.c
HEADER  = mytypes.h bitcoin_functions.h OrderedLinkedList.h
OUT  	= bitcoin
CC	= gcc
FLAGS   = -c -Wall

$(OUT): $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

bitcoin.o: bitcoin.c
	$(CC) $(FLAGS) bitcoin.c

bitcoin_functions.o: bitcoin_functions.c
	$(CC) $(FLAGS) bitcoin_functions.c

clean:
	rm -f $(OBJS) $(OUT)
