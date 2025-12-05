#include "Optimal_algorithm.h"
#include <algorithm>
#include <iostream>

using namespace std;

Optimal_algorithm::Optimal_algorithm(char symbol) : symbol(symbol){}

int Optimal_algorithm::evaluate(BOARD &b, char player, char opponent){
    // Checking for Rows
    for(int row = 0; row<3; row++){
        if(b.get_cell(row, 0) == b.get_cell(row, 1) &&
            b.get_cell(row, 1) == b.get_cell(row, 2))
        {
            if(b.get_cell(row, 0) == player) return +10;
            else if(b.get_cell(row,0) == opponent) return -10;
        }
    }
    // Checking for Columns
    for(int col = 0; col<3; col++){
        if(b.get_cell(0, col) == b.get_cell(1, col) &&
            b.get_cell(1, col) == b.get_cell(2, col))
        {
            if(b.get_cell(0, col) == player) return +10;
            else if(b.get_cell(0, col) == opponent) return -10;
        }
    }
    // Checking for Diagonals
    if(b.get_cell(0,0) == b.get_cell(1,1) && b.get_cell(1,1) == b.get_cell(2,2)) {
        if(b.get_cell(0, 0) == player) return +10;
        else if(b.get_cell(0,0) == opponent) return -10;
    }

    if(b.get_cell(0,2) == b.get_cell(1,1) && b.get_cell(1,1) == b.get_cell(2,0)) {
        if(b.get_cell(0,2) == player) return +10;
        else if(b.get_cell(0,2) == opponent) return -10;
    }
    
    return 0;
}

int Optimal_algorithm::minimax(BOARD board, int depth, bool isMax, char algorithm, char bot){
    int score = evaluate(board, algorithm, bot);
    
    if(score == 10) return score - depth;
    if(score == -10) return score + depth;
    if(!board.isMoveLeft()) return 0;

    if(isMax){
        int best = -1000;
        for(int i = 0; i<3; i++){
            for(int j = 0; j<3; j++){
                if(board.get_cell(i, j) == ' '){
                    BOARD nb = board;
                    nb.make_move(algorithm, i, j);
                    best = max(best, minimax(nb, depth+1, false, algorithm, bot));
                }
            }
        }
        return best;
    } else {
        int best = 1000;
        for(int i = 0; i<3; i++){
            for(int j = 0; j<3; j++){
                if(board.get_cell(i, j) == ' '){
                    BOARD nb = board;
                    nb.make_move(bot, i, j);
                    best = min(best, minimax(nb, depth+1, true, algorithm, bot));
                }
            }
        }
        return best;
    }
}

Optimal_algorithm::Move Optimal_algorithm::findBestMove(BOARD &board, char algorithm, char bot){
    int bestVal = -1000;
    Move bestMove = {-1, -1};

    for(int i = 0; i<3; i++){
        for(int j = 0; j<3; j++){
            if(board.get_cell(i, j) == ' '){
                BOARD nb = board;
                nb.make_move(algorithm, i, j);
                int moveVal = minimax(nb, 0, false, algorithm, bot);
                if(moveVal > bestVal){
                    bestVal = moveVal;
                    bestMove = {i, j};
                }
            }
        }
    }
    return bestMove;
}