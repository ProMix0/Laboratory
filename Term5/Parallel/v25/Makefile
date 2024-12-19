.PHONY: run

Q=@
SEED=15
COUNT=100
TIME=50

run: serial parallel
	$Qecho Serial execution
	$Q./serial $(COUNT) $(TIME)
	$Qecho Parallel execution
	$Q./parallel $(COUNT) $(TIME)
	$Qecho Difference:
	$Q-diff result_serial result_parallel

serial: serial.cpp
	$Qg++ $< -o serial
parallel: parallel.cu
	$Q/usr/local/cuda/bin/nvcc $< -o parallel

