#Name: Suraj Garg
#Roll No: 2018202003

CC = g++
CFLAGS = -g -Wall -lcrypto -std=c++1z
DEPS = mtorrentCreator.h computeSHA.h connection.h utilities.h
OBJ = mtorrentCreator.o computeSHA.o connection.o utilities.o torrentClient.o
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< 

torrentClient: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lcrypto -lpthread

