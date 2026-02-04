#include "Board.h" 
using namespace std;

/**
 * @brief Constructs an empty Tic-Tac-Toe board.
 */
BOARD::BOARD() : used_cells(0), grid(9) {
    reset_board();
}

/**
 * @brief Resets the board to its default settings.
 */
void BOARD::reset_board(void) {
    for (auto& symbol : grid)
        symbol = EMPTY_CELL;
    used_cells = 0;
}

/**
 * @brief Checks if a move is valid given the current board state.
 * @param x x coordinate (int between 0 and 2).
 * @param y y coordinate (int between 0 and 2).
 * @returns TRUE if the move is valid, FALSE otherwise.
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
 * @brief Checks if there are any valid moves left on the board.
 * @returns TRUE if another move can be made, FALSE otherwise.
 */
bool BOARD::move_available(void){
    for(int i = 0; i<9; i++) 
        if(grid[i] == EMPTY_CELL)
            return true;
    return false;
}

/**
 * @brief Checks if the board is full (resulting in a draw).
 * @returns TRUE if the board is full, FALSE otherwise.
 */
bool BOARD::full(void) {
    return used_cells == 9;
}

/**
 * @brief Places a player's symbol on the board.
 * @param player the player's symbol
 * @param x x coordinate (int between 0 and 2).
 * @param y y coordinate (int between 0 and 2).
 * @returns FALSE if there aren't any valid moves left, TRUE if a move is made.
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
 * @returns TRUE if the last player won, FALSE otherwise.
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
 * @param x x coordinate (int between 0 and 2).
 * @param y y coordinate (int between 0 and 2).
 * @returns '?' if the coordinates are invalid.
 */
char BOARD::get_cell(short int x, short int y) const {
    if (x < 0 || x > 2 || y < 0 || y > 2) {
        return '?'; 
    }
    // Converts (x, y) to the linear vector index (x * 3 + y)
    return grid[x * 3 + y];
}