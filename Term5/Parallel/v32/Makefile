.PHONY: run all parallel

all: serial parallel

serial: serial.c
	$(CC) serial.c -o serial

parallel: parallel.c
	mpicc parallel.c -o _parallel

