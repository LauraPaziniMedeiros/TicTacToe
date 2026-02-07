#ifndef OPTIMAL_H
    #define OPTIMAL_H
    #include "Bot.h"

    /**
     * @brief A minimax algorithm to choose the best movement in a tic tac toe game.
     */
    class Optimal {
        private:
        char opponent;
        static int evaluate(BOARD& board, const char& symbol);
        static int minimax(BOARD& board, int depth, bool is_max, const char& symbol, 
        const char& opponent, int alpha, int beta);

        public:
        static pair<short, short> find_best_move(BOARD board, char& symbol);
    };
#endif