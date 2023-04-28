CC = gcc
SERVER = server 
CLIENT = client 

#OBJS = $(patsubst %.c, %.o, $(wildcard *.c))
CFLAGS += -Wall -O2 -g -fPIC -std=gnu99 -DMYDEB
LDFLAGS = -lpthread

SERVER_OBJS = server*.o
SERVER_SRC = server*.c
CLIENT_OBJS = client*.o
CLIENT_SRC = client*.c

$(SERVER) : $(SERVER_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

$(SERVER_OBJS) : $(SERVER_SRC)
	$(CC) $(CFLAGS) -c  $^

$(CLIENT) : $(CLIENT_OBJS)
	$(CC) -o $@ $^ -lsqlite3

$(CLIENT_OBJS) : $(CLIENT_SRC)
	$(CC) $(CFLAGS) -c $^

all : $(SERVER) $(CLIENT) copy

copy:
	cp $(SERVER) ${CLIENT} ../bin/ 
	cp $(CLIENT) ../clients/client 
	cp $(CLIENT) ../clients/client1 

clean:
	rm $(SERVER_OBJS) $(CLIENT_OBJS) $(SERVER) $(CLIENT)

