all:
	g++ genetic_algorithm.cpp population.cpp -o a -Wall -Werror

run: all
	./a

clean:
	rm a *.txt