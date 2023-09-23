.PHONY: build

%: v%.c v%_lib.c
	gcc -shared v$@_lib.c -o libv$@.so
	gcc child_wrapper.c v$@.c