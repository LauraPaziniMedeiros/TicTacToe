all:
	g++ genetic_algorithm.cpp population.cpp -o a -Wall -Werror

run: all
	./a >output.txt 2>&1

clean:
	rm a *.txt