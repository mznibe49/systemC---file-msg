CPP=gcc
CFLAGS=-Wall -g -lrt -lpthread
EXEC=projet
SRC= $(wildcard *.c)
OBJ= $(SRC:.c=.o)

all: $(EXEC)

projet: msg_file.o test1.o initStructs.o 
	@$(CPP) -o $@ $^ $(CFLAGS)

main.o: msg_file.h structs.h initStructs.h

%.o: %.c
	@$(CPP) -o $@ -c $< $(CFLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf $(EXEC)
