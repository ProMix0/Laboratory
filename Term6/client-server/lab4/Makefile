.PHONY: all build clean run_s run_c

all: build

build:
	mkdir -p storage/server_storage
	mvn compile

run_s:
	mvn exec:java -Dexec.mainClass=app.Server || true

run_c:
	mvn exec:java -Dexec.mainClass=app.Client

clean:
	rm -rf target storage classes
