.PHONY: run all parallel

all: serial parallel

serial: serial.c
	$(CC) -std=c99 serial.c -o serial

parallel: parallel.c
	mpicc -std=c99 parallel.c -o _parallel

