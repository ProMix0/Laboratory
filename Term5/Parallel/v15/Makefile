.PHONY: run all

all: serial parallel gendata

Q=@

serial: serial.c
	$(Q)$(CC) -std=c99 serial.c -o serial

parallel: parallel.c
	$(Q)$(CC) -std=c99 parallel.c -o parallel -fopenmp

gendata: gendata.c
	$(Q)$(CC) gendata.c -o gendata

run: all
	@echo Remember to set N, COUNT1 and COUNT2 variables
	$(Q)./gendata $N $(COUNT1) $N$(COUNT1)$(COUNT2) > polynom1
	$(Q)./gendata $N $(COUNT2) $N$(COUNT1)$(COUNT2) > polynom2
	@echo Serial version:
	$(Q)./serial $N polynom1 polynom2 2> result_serial
	@echo Parallel version:
	$(Q)./parallel $N polynom1 polynom2 2> result_parallel
	@echo Difference between results:
	$(Q)diff result_serial result_parallel

