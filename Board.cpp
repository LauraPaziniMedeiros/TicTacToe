#include "Board.h" 
#include <iostream>
#include <vector>

using namespace std;

// O conteúdo da classe BOARD { ... } FOI REMOVIDO daqui.
// Apenas as implementações dos métodos são mantidas.

/**
 * @brief Constructs an empty Tic-Tac-Toe board.
 */
BOARD::BOARD() : used_cells(0), grid(9) {
    reset_board();
}

/**
 * @brief Resets the board to its default settings
 */
void BOARD::reset_board(void) {
    for (auto& symbol : grid)
        symbol = EMPTY_CELL;
    used_cells = 0;
}

/**
 * @brief Checks if a move is valid.
 */
bool BOARD::valid_move(short int x, short int y) {
    // Move is out of bounds
    if(x < 0 || x > 2 || y < 0 || y > 2)
        return false;

    // Cell is already used
    if(grid[x*3 + y] != EMPTY_CELL)
        return false;

    return true;
}

/**
 * @brief Prints the current board state to the console.
 */
void BOARD::draw_board(void) {
    cout << "-------------" << endl;
    for (int i = 0; i < 3; i++) {
        cout << "| ";
        for (int j = 0; j < 3; j++) {
            cout << grid[i*3 + j] << " | ";
        }
        cout << endl << "-------------" << endl;
    }
}

/**
 * @brief Checks if there are moves left on the board.
 */
bool BOARD::isMoveLeft(void){
    for(int i = 0; i<9; i++) 
        if(grid[i] == EMPTY_CELL)
            return true;
    return false;
}

/**
 * @brief Checks if the board is full (resulting in a draw).
 */
bool BOARD::full(void) {
    return used_cells == 9;
}

/**
 * @brief Places a player's symbol on the board.
 */
bool BOARD::make_move(char player, short int x, short int y) {
    if(!valid_move(x, y))
        return false;      
    
    grid[x*3 + y] = player;
    used_cells++;
    return true;
}

/**
 * @brief Checks if the last move resulted in a win.
 */
bool BOARD::check_win(short int x, short int y) {
    char symbol = grid[x*3 +y];
    if(symbol == EMPTY_CELL)
        return false;

    // Horizontal win
    if(grid[x*3] == symbol && grid[x*3 + 1] == symbol && grid[x*3 + 2] == symbol)
        return true;

    // Vertical win
    if(grid[y] == symbol && grid[3 + y] == symbol && grid[6 + y] == symbol)
        return true;

    // Diagonal win
    if(grid[0] != EMPTY_CELL && grid[0] == grid[4] && grid[0] == grid[8])
        return true;
    if(grid[2] != EMPTY_CELL && grid[2] == grid[4] && grid[4] == grid[6])
        return true;

    return false;
}

/**
 * @brief Gets the symbol at a specific cell.
 */
char BOARD::get_cell(short int x, short int y) const {
    if (x < 0 || x > 2 || y < 0 || y > 2) {
        return '?'; 
    }
    // Converts (x, y) to the linear vector index (x * 3 + y)
    return grid[x * 3 + y];
}