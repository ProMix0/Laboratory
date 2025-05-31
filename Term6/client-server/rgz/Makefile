JAVAC=javac
SOURCES=*.java

.PHONY: all build run_s run_c

all: build

build:
	$(JAVAC) $(SOURCES)

run_s:
	java Server || true

run_c:
	java Client

