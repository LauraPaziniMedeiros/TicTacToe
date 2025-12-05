#ifndef OPTIMAL_ALGORITHM_H
#define OPTIMAL_ALGORITHM_H

#include "Board.h"

class Optimal_algorithm{
    private:
        int evaluate(BOARD &b, char player, char opponent);
        int minimax(BOARD board, int depth, bool isMax, char algorithm, char bot);

    public:
        struct Move {
            int row, col;
        };
        char symbol;
        Optimal_algorithm(char symbol = 'O');
        Move findBestMove(BOARD &board, char algorithm, char bot);
};

#endif