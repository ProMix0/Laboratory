TARGET := a.out
CC = clang
CFLAGS = -O2
CPU_NUM = $(shell nproc)

.PHONY: $(TARGET)
$(TARGET):
	$(CC) $(CFLAGS) *.c -o $@ -lm -fopenmp -DCPU_NUM=$(CPU_NUM)

run: $(TARGET)
	time ./$(TARGET)
	time ./$(TARGET)
	time ./$(TARGET)

#test: $(TARGET)

format:
	clang-format -i *.c *.h

