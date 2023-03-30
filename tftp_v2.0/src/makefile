CC = gcc
SERVER = server 
CLIENT = client 

#OBJS = $(patsubst %.c, %.o, $(wildcard *.c))
CFLAGS += -Wall -O2 -g -fPIC -std=gnu99 -DMYDEB

SERVER_OBJS = server*.o
SERVER_SRC = server*.c
CLIENT_OBJS = client*.o
CLIENT_SRC = client*.c

$(SERVER) : $(SERVER_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(SERVER_OBJS) : $(SERVER_SRC)
	$(CC) $(CFLAGS) -c  $^

$(CLIENT) : $(CLIENT_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(CLIENT_OBJS) : $(CLIENT_SRC)
	$(CC) $(CFLAGS) -c $^

copy:
	cp $(SERVER) ${CLIENT} ../bin/ 
	cp $(CLIENT) ../client/ 

clean:
	rm $(SERVER_OBJS) $(CLIENT_OBJS) $(SERVER) $(CLIENT)

