#Makefile

CC = cc
FILE = main.o prog.o
CFLAG = -Wall
EXE = mypro

$(EXE): $(FILE)
	$(CC) $(FILE) -o $(EXE)
	@echo "Created"

main.o: main.c header.h
	$(CC) -c main.c
prog.o: prog.c header.h
	$(CC) -c prog.c

clear:
	@rm -f $(FILE) $(EXE)
	@echo "Cleared"


