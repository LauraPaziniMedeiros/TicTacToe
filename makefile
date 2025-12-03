all:
	g++ genetic_with_greater.cpp population.cpp -o a -Wall -Werror

run:
	./a

clean:
	rm a *.txt