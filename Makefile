all: forth run
build:
	$(CC) main.c parser.c type.c context.c func.c -Wall -w -O2 -o forth
run:
	./forth program.forth
clean:
	rm forth
rebuild: clean build run
shell:
	./forth
