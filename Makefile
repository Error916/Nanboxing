CC=gcc
CFLAGS=-pedantic -Wall -O3
LIBS=


nan: nan.c
	$(CC) $(CFLAGS) -o nan nan.c $(LIBS)
