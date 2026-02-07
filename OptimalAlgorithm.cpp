#include "header_files/OptimalAlgorithm.h"

/**
 * @brief Generates a score based on the possibility of a win, draw or loss
 * in the next movement.
 * @param board the current state of the board.
 * @param symbol the player's symbol.
 * @returns WIN: +10, LOSS: -10, DRAW: 0.
 */
int Optimal::evaluate(BOARD& board, const char& symbol) {
    // Checking for Rows
    for (int row = 0; row<3; row++)
    {
        if (board.grid[row*3] == board.grid[row*3 + 1] && board.grid[row*3 + 1] == board.grid[row*3 + 2]) {
            if (board.grid[row*3] == symbol)
                return +10;
            else if (board.grid[row*3] != EMPTY_CELL)
                return -10;
        }
    }

    // Checking for Columns
    for (int col = 0; col<3; col++) {
        if (board.grid[col] == board.grid[3 + col] && board.grid[3 + col] == board.grid[6 + col]) {
            if (board.grid[col] == symbol)
                return +10;
            else if (board.grid[col] != EMPTY_CELL)
                return -10;
        }
    }

    // Checking for Diagonals
    if (board.grid[0] == board.grid[4] && board.grid[4] == board.grid[8]) {
        if (board.grid[0] == symbol)
            return +10;
        else if (board.grid[0] != EMPTY_CELL)
            return -10;
    }
    if (board.grid[2] == board.grid[4] && board.grid[4] == board.grid[6])
    {
        if (board.grid[2] == symbol)
            return +10;
        else if (board.grid[2] != EMPTY_CELL)
            return -10;
    }
    // No win possible
    return 0;
}

/**
 * @brief Generates the minimizer or maximizer best score using recursion.
 * @param board The current state of the board.
 * @param depth The depth of the current possibility tree.
 * @param symbol The symbol of the player being analized
 * @returns The best possible score for this board state.
 */
int Optimal::minimax(BOARD& board, int depth, bool is_max, const char& symbol, 
    const char& opponent, int alpha, int beta) {
    int score = evaluate(board, symbol);

    // Minimizer, maximizer or draw 
    if(score == 10) return score - depth;
    if(score == -10) return score + depth;
    if(!board.move_available()) return 0;

    // Maximizer's move
    if (is_max) {
        int best = -1000;
        // Traverse all cells
        for (int i = 0; i<3; i++) {
            for (int j = 0; j<3; j++) {
                // Check if cell is empty
                if (board.grid[i*3 + j] == EMPTY_CELL) {
                    // Make the move
                    board.grid[i*3 + j] = symbol;
                    // Call minimax recursively and choose the maximum value
                    int val = minimax(board, depth+1, !is_max, symbol, opponent, alpha, beta);
                    best = max(best, val);
                    alpha = max(alpha, best);
                    // Undo the move
                    board.grid[i*3 + j] = EMPTY_CELL;
                    // PRUNING: Beta Cut-off
                    // If the maximizer found a move better than the minimizer
                    if (beta <= alpha) break;
                }
            }
            if (beta <= alpha) break;
        }
        return best;
    } else {
        int best = 1000;
        // Traverse all cells
        for (int i = 0; i<3; i++) {
            for (int j = 0; j<3; j++) {
                // Check if cell is empty
                if (board.grid[i*3 + j] == EMPTY_CELL) {
                    // Make the move
                    board.grid[i*3 + j] = opponent;
                    // Call minimax recursively and choose the maximum value
                    int val = minimax(board, depth+1, !is_max, symbol, opponent, alpha, beta);
                    best = min(best, val);
                    alpha = min(alpha, best);
                    // Undo the move
                    board.grid[i*3 + j] = EMPTY_CELL;
                    // PRUNING: Alpha Cut-off
                    if (beta <= alpha) break;
                }
            }
            if (beta <= alpha) break;
        }
        return best;
    }
}

/**
 * @brief Finds the best possible move for a player.
 * @param board The current state of the board.
 * @param symbol The player's symbol on the board
 * @returns The best movement that leads to a win/draw.
 */
pair<short, short> Optimal::find_best_move(BOARD board, char& symbol) {
    char opponent = symbol == 'X' ? 'O' : 'X';
    int bestVal = -1000;
    pair<short, short> bestMove;
    bestMove.first = -1;
    bestMove.second = -1;
    // Traverse all cells, evaluate minimax function for
    // all empty cells. And return the cell with optimal value.
    for (int i = 0; i<3; i++) {
        for (int j = 0; j<3; j++) {
            // Check if cell is empty
            if (board.grid[i*3 + j] == EMPTY_CELL) {
                // Make the move
                board.grid[i*3 + j] = symbol;

                int moveVal = minimax(board, 0, false, symbol, opponent, -1000, 1000);

                // Undo the move
                board.grid[i*3 + j] = EMPTY_CELL;

                if (moveVal > bestVal) {
                    bestMove.first = i;
                    bestMove.second = j;
                    bestVal = moveVal;
                }
            }
        }
    }
    return bestMove;
}