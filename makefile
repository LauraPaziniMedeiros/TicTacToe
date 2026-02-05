all:
	g++ Board.cpp Bot.cpp Optimal_algorithm.cpp Play.cpp population.cpp -o game -Wall -Werror

run: all
	./game

runtxt: all
	./game

clean:
	rm game results/*.txt