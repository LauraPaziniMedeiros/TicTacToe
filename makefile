all:
	g++ genetic_algorithm.cpp -o a -Wall -Werror

run:
	./a

clean:
	rm a