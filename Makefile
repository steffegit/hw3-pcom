CC = g++
CFLAGS = -Wall

all: client

client: client.cpp requests.cpp helpers.cpp
	$(CC) $(CFLAGS) -o client client.cpp requests.cpp helpers.cpp 

clean:
	rm -f *.o client
