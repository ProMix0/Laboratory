.PHONY: run all parallel

Q=@
N=978705920

run: all
	@echo Serial version:
	$(Q)./serial $N
	@echo Parallel version:
	$(Q)./parallel $N

all: serial parallel

serial: serial.c
	$(Q)$(CC) -std=c99 serial.c -o serial -lm

parallel: parallel.c
	$(Q)$(CC) -std=c99 parallel.c -o parallel -lm -fopenmp

