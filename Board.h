#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <iostream>
#include <map>
#include <string>

using namespace std;

// --- CONSTANTES ---
const char EMPTY_CELL = ' '; // Defines an empty cell for the game board
#define WIN 1
#define LOSS -1
#define DRAW 0

/**
 * @class BOARD
 * @brief Manages the 3x3 grid state for a Tic-Tac-Toe game.
 */
class BOARD {
private:
    short int used_cells;
public:
    vector<char> grid;

    BOARD();
    void reset_board(void);
    
    // Protótipos dos Métodos
    bool valid_move(short int x, short int y);
    void draw_board(void);
    bool isMoveLeft(void);
    bool full(void);
    bool make_move(char player, short int x, short int y);
    bool check_win(short int x, short int y);
    char get_cell(short int x, short int y) const;
};

#endif // BOARD_H