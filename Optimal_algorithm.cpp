#include "Board.h"

class Optimal_algorithm{
    private:
        int evaluate(BOARD &b, char player, char opponent){
        // Checking for Rows for X or O victory.
        for(int row = 0; row<3; row++){
            if(b.get_cell(row, 0) == b.get_cell(row, 1) &&
                b.get_cell(row, 1) == b.get_cell(row, 2))
            {
                if(b.get_cell(row, 0) == player)
                    return +10;
                else if(b.get_cell(row,0) == opponent)
                    return -10;
            }
        }
        // Checking for Columns for X or O victory.
        for(int col = 0; col<3; col++){
            if(b.get_cell(0, col) == b.get_cell(1, col) &&
                b.get_cell(1, col) == b.get_cell(2, col))
            {
                if(b.get_cell(0, col) == player)
                    return +10;
                else if(b.get_cell(0, col) == opponent)
                    return -10;
            }
        }

        // Checking for Diagonals for X or O victory.
        if(b.get_cell(0,0) == b.get_cell(1,1) && 
            b.get_cell(1,1) == b.get_cell(2,2))
        {
            if(b.get_cell(0, 0) == player)
                return +10;
            else if(b.get_cell(0,0) == opponent)
                return -10;
        }

        if(b.get_cell(0,2) == b.get_cell(1,1) && 
            b.get_cell(1,1) == b.get_cell(2,0))
        {
            if(b.get_cell(0,2) == player)
                return +10;
            else if(b.get_cell(0,2) == opponent)
                return -10;
        }
        
        // Else if none of them have won then return 0
        return 0;
    }

    /*This is the minimax function. It considers all
    the possible ways the game can go and returns
    the value of the board*/
    int minimax(BOARD board, int depth, bool isMax, char algorithm, char bot){
        int score = evaluate(board, algorithm, bot);
        
        /*If Maximizer has won the game return his/her
        evaluated score*/
        if(score == 10)
            return score - depth;

        /*If Minimizer has won the game return his/her
        evaluated score*/
        if(score == -10)
            return score + depth;
        /*If there are no more moves and no winner then
        it is a tie*/
        if(!board.isMoveLeft())
            return 0;

        // If this maximizer's move
        if(isMax){
            int best = -1000;

            // Traverse all cells
            for(int i = 0; i<3; i++){
                for(int j = 0; j<3; j++){

                    // Check if cell is empty
                    if(board.get_cell(i, j) == ' '){
                        // Make the move
                        BOARD nb = board;
                        nb.make_move(algorithm, i, j);

                         // Call minimax recursively and choose
                        // the maximum value
                        best = max(best, minimax(nb, depth+1, false, algorithm, bot));
                    }
                }
            }
            return best;
        }

        // If this minimizer's move
        else{
            int best = 1000;

            // Traverse all cells
            for(int i = 0; i<3; i++){
                for(int j = 0; j<3; j++){
                     // Check if cell is empty
                    if(board.get_cell(i, j) == ' '){
                        // Make the move
                        BOARD nb = board;
                        nb.make_move(bot, i, j);


                        // Call minimax recursively and choose
                        // the minimum value
                        best = min(best, minimax(nb, depth+1, true, algorithm, bot));
                    }
                }
            }
            return best;
        }
    }

public:

    struct Move {
        int row, col;
    };
    char symbol;
    Optimal_algorithm(char symbol = 'O') : symbol(symbol){}

    // This will return the best possible move
    Move findBestMove(BOARD &board, char algorithm, char bot){
        int bestVal = -1000;
        Move bestMove = {-1, -1};

        /*Traverse all cells, evaluate minimax function for
        all empty cells. And return the cell with optimal value.*/
        for(int i = 0; i<3; i++){
            for(int j = 0; j<3; j++){
                // Check if cell is empty
                if(board.get_cell(i, j) == ' '){
                    // Make the move
                    BOARD nb = board;
                    nb.make_move(algorithm, i, j);

                    // compute evaluation function for this move.
                    int moveVal = minimax(nb, 0, false, algorithm, bot);

                    /*If the value of the current move is 
                    more than the best value, then update best*/
                    if(moveVal > bestVal){
                        bestVal = moveVal;
                        bestMove = {i, j};
                    }
                }
            }
        }
        return bestMove;
    }
};
