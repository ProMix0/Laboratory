TARGET := a.out
CC = clang
CFLAGS = -O2
CPU_NUM = $(shell nproc)

.PHONY: $(TARGET)
$(TARGET):
	$(CC) $(CFLAGS) *.c -o $@ -lm -fopenmp -DCPU_NUM=$(CPU_NUM)

run: $(TARGET)
	time ./$(TARGET) # < test.csv > /dev/null

test:
	cat rt_iot.csv > test.csv
	for i in $$(seq 150); do	\
		sed 1d rt_iot.csv >> test.csv;	\
	done

format:
	clang-format -i *.c *.h

