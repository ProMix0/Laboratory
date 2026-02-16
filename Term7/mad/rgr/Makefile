TARGET := a.out
CC = clang
CFLAGS = -g -Og

.PHONY: $(TARGET)
$(TARGET):
	$(CC) $(CFLAGS) *.c -o $@ -lm -fopenmp

run: $(TARGET)
	time ./$(TARGET) < test.csv > out.csv

test:
	cat rt_iot.csv > test.csv
	for i in $$(seq 10); do	\
		sed 1d rt_iot.csv >> test.csv;	\
	done

format:
	clang-format -i *.c *.h

