#OBJS = $(patsubst %.c, %.o, $(wildcard %.c))

SRC = thr.c main.c
OBJS = *.o
APP = app
CC = gcc
LDFLAGS = -lpthread

$(APP) : $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)
$(OBJS) : $(SRC)
	$(CC) -c $^

all : $(APP) run

clean:
	rm $(OBJS) $(APP)
run:
	./$(APP)
