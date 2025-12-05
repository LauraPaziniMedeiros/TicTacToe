all:
	g++ Board.cpp Bot.cpp Optimal_algorithm.cpp Play.cpp population.cpp main.cpp -o a -Wall

run: all
	./a

clean:
	rm a *.txt