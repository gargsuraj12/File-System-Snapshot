
CC = g++
CFLAGS = -Wall -std=c++1z
DEPS = headerFiles.h normalMode.h fileOpen.h deleteFolder.h createFile.h renameFile.h deleteFile.h copyFile.h createDirectory.h snapshot.h searchFile.h
OBJ = Application.o
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

Application: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

run: 
	@./Application

