.PHONY: run

Q=@
SEED=15

run: serial parallel
	$Qecho Serial execution
	$Q./serial 100 50
	$Qecho Parallel execution
	$Q./parallel 100 50
	$Qecho Difference:
	$Q-diff result_serial result_parallel

serial: serial.cpp
	$Qg++ $< -o serial
parallel: parallel.cu
	$Q/usr/local/cuda/bin/nvcc $< -o parallel

