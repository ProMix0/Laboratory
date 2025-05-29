JAVAC=javac
SRC=src
BIN=classes
STORAGE = storage
SOURCES=src/*.java

.PHONY: all build clean run_s run_c

all: build

build:
	mkdir -p $(BIN) $(STORAGE)/server_storage 
	$(JAVAC) -d $(BIN) $(SOURCES)

run_s:
	java -cp $(BIN) Server || true

run_c:
	java -cp $(BIN) Client

clean:
	rm -rf $(BIN) $(STORAGE)
