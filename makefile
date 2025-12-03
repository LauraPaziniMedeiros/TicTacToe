all:
	g++ genetic_with_greater.cpp population.cpp -o a -Wall -Werror

run:
	./a

clean:
	rm a O0.txt O2.txt O4.txt X0.txt X2.txt X4.txt