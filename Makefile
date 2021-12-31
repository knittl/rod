.PHONY: clean test build docker

rod: rod.c
	cc $^ -o $@ -Wall -Wextra -Werror -ansi -pedantic -O3 $(CCFLAGS)

test: clean build
	./tests.sh

build: rod

docker: rod.c
	docker build -t knittl/rod .

clean:
	rm -f rod
