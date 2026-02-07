all:
	g++ Board.cpp Random64.cpp Bot.cpp OptimalAlgorithm.cpp TicTacToe.cpp Population.cpp main.cpp -o game -Wall -Werror

run: all
	./game

clean:
	rm game

purge:
	rm results/* game