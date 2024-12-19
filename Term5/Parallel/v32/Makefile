.PHONY: run all parallel

Q=@
N=1000000
LEN=100

run: all
	$Qecho Serial:
	$Q./serial $N $(LEN)
	$Qecho Parallel:
	$Qmpiexec -n "$$(nproc)" ./parallel $N $(LEN)

all: serial parallel

serial: serial.c
	$Q$(CC) -std=c99 serial.c -o serial

parallel: parallel.c
	$Qmpicc -std=c99 parallel.c -o parallel

