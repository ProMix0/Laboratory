.PHONY: run

Q=@
SEED=15
COUNT=100000

run: serial parallel
	$Qecho Serial execution
	$Q./serial $(COUNT)
	$Qecho Parallel execution
	$Q./parallel $(COUNT)
	$Qecho Difference:
	$Q-diff result_serial result_parallel

serial: serial.cpp
	$Qgcc $< -o serial
parallel: parallel.cpp
	$Qg++ $< -o parallel -lOpenCL -I/usr/local/cuda/include/ -L/usr/local/cuda/lib64/

