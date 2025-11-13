#include <iostream>
#include <array>
using namespace std;

/**
 * @class Board
 * @brief Manages the 3x3 grid state for a Tic-Tac-Toe game.
 *
 * This class is responsible for storing the board,
 * making moves, and checking for win/draw conditions.
 */
class BOARD {
private:
    array<array<char, 3>, 3> grid; // ' ', 'X' or 'O'
    short int used_cells; // Checks for a draw
    const char EMPTY_CELL = ' ';

public:
    /**
     * @brief Constructs an empty Tic-Tac-Toe board.
     */
    BOARD() : used_cells(0) {
        for (auto& row : grid) {
            row.fill(EMPTY_CELL);
        }
    }

    /**
     * @brief Checks if a move is valid.
     * @param x The row index (0-2).
     * @param y The column index (0-2).
     * @return true if the move is within bounds and the cell is empty, false otherwise.
     */
    bool valid_move(short int x, short int y) {
        // Move is out of bounds
        if(x < 0 || x > 2)
            return false;
        if(y < 0 || y > 2)
            return false;

        // Cell is already used
        if(grid[x][y] != EMPTY_CELL)
            return false;

        return true;
    }

    /**
     * @brief Prints the current board state to the console.
     */
    void draw_board(void) {
        cout << "-------------" << endl;
        for (int i = 0; i < 3; i++) {
            cout << "| ";
            for (int j = 0; j < 3; j++) {
                cout << grid[i][j] << " | ";
            }
            cout << endl << "-------------" << endl;
        }
    }

    /**
     * @brief Checks if the board is full (resulting in a draw).
     * @return true if all 9 cells are used, false otherwise.
     */
    bool full(void) {
        return used_cells == 9;
    }

    /**
     * @brief Places a player's symbol on the board.
     * @param player The player's symbol ('X' or 'O').
     * @param x The row index (0-2).
     * @param y The column index (0-2).
     * @return true if the move was successfully made, false if it was invalid.
     */
    bool make_move(char player, short int x, short int y) {
        if(!valid_move(x, y))
            return false;      
        
        grid[x][y] = player;
        used_cells++;
        return true;
    }

    /**
     * @brief Checks if the last move resulted in a win.
     * @param x The row of the last move.
     * @param y The column of the last move.
     * @return true if the last move created a 3-in-a-row, false otherwise.
     */
    bool check_win(short int x, short int y) {
        char symbol = grid[x][y];
        if(symbol == EMPTY_CELL)
            return false;

        // Horizontal win
        if(grid[x][0] == symbol && grid[x][1] == symbol && grid[x][2] == symbol)
            return true;

        // Vertical win
        if(grid[0][y] == symbol && grid[1][y] == symbol && grid[2][y] == symbol)
            return true;

        // Diagonal win
        if(grid[0][0] != EMPTY_CELL && grid[0][0] == grid[1][1] && grid[0][0] == grid[2][2])
            return true;
        if(grid[0][2] != EMPTY_CELL && grid[0][2] == grid[1][1] && grid[1][1] == grid[2][0])
            return true;

        return false;
    }
};

/**
 * @class TicTacToe
 * @brief Manages the tic tac toe game logic and player interaction.
 */
class TicTacToe {
private:
    BOARD board;
    array<char, 2> players; // Stores each player's symbols
    bool curr_player; // 0 = p0, 1 = p1

    /**
     * @brief Switches the current player.
     */
    void switch_player() {
        curr_player = !curr_player; // Toggles 0 and 1
    }

public:
    /**
     * @brief Constructs a new game.
     * @param p0 Symbol for Player 1 (default 'X').
     * @param p1 Symbol for Player 2 (default 'O').
     */
    TicTacToe(char p0 = 'X', char p1 = 'O') : board(), players{p0, p1}, curr_player(0) {}

    /**
     * @brief Runs the main game loop.
     * @param print Toggles console output (true for human play, false for testing).
     */
    void play(bool print = true) {
        short int x, y; // Player's moves (grid index)
        
        while(true) {
            if(print)
                board.draw_board();

            // Gets and validates the current player's input
            if(print)
                cout << "Player " << players[curr_player] << ", make a move (row and collum): ";
            cin >> x >> y;
            while(!board.make_move(players[curr_player], x, y)) {
                if(print) {
                    cout << "Unvalid move.\n";
                    cout << "Player " << players[curr_player] << ", make a move (row and collum): ";
                }                
                cin >> x >> y;
            }

            // Stops the game if the current player won
            if(board.check_win(x, y)) {
                if(print) {
                    board.draw_board();
                    cout << "You won!\n";
                }
                return;const char EMPTY_CELL = ' ';
            }

            // Stops the game if the last move filled up the board (draw);
            if(board.full()) {
                if(print) {
                    board.draw_board();
                    cout << "It's a draw!\n";
                }
                return;
            }

            switch_player();
        }
    }
};

int main() {
    TicTacToe game;
    game.play();
    return 0;
}