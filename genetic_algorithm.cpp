#include <iostream>
#include <array>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <random>
#include <fstream>
using namespace std;

const char EMPTY_CELL = ' '; // Defines an empty cell for the game board
#define WIN 1
#define LOSS -1
#define DRAW 0


/**
 * @class Board
 * @brief Manages the 3x3 grid state for a Tic-Tac-Toe game.
 *
 * This class is responsible for storing the board,
 * making moves, and checking for win/draw conditions.
 */
class BOARD {
private:
    short int used_cells; // Checks for a draw

public:
    vector<char> grid; // ' ', 'X' or 'O'

    void initialize_board(void) {
        for (auto& symbol : grid)
            symbol = EMPTY_CELL;
        used_cells = 0;
    }

    /**
     * @brief Constructs an empty Tic-Tac-Toe board.
     */
    BOARD() : used_cells(0), grid(9) {
        initialize_board();
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
        if(grid[x*3 + y] != EMPTY_CELL)
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
                cout << grid[i*3 + j] << " | ";
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
        
        grid[x*3 + y] = player;
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
};

/**
 * @class BOT
 * @brief Manages a auto-player for the tic tac toe game.
 *
 * This class is responsible for managing the genomes of an evolving bot
 * to ensure it always wins/draws in a game of tic tac toe.
 */
class BOT {
    private:
    // 'genomes' maps a board state to a vector of 9 scores (one for each cell of the board)
    map<vector<char>, vector<int>> genomes;
    // 'last_game' stores the sequence of board states for this bot in the current game
    vector<vector<char>> last_game;
    // 'moves' stores the {x, y} coordinates for each move in 'last_game'
    vector<pair<short, short>> moves;

    public:
    // The bot's symbol on the board
    char symbol;

    BOT(char symbol) : symbol(symbol) {}

    /**
     * @brief registers the last move made by the bot.
     * @param grid the board's current grid.
     * @param x the row of the last move.
     * @param y the columm of the last move.
     */
    void register_move(const vector<char>& grid, const short& x, const short& y) {
        last_game.push_back(grid);
        moves.push_back({x, y});
    }

    /**
     * @brief Updates the bot's "genome" based on the game's outcome.
     * @param result Represents the result of the game: 
     * 1 if the bot won, -1 if it lost and 0 if it's a draw.
     */
    void update_genomes(short result) {
        int counter = 0;
        int reward = 0;

        if(result == WIN) {
            reward = 10;
        } else if(result == LOSS) {
            reward = -10;
        } else if(result == DRAW) {
            reward = 5; // Give a smaller reward for drawing to prefer it over losing
        }

        // Apply reward to all moves made in the game
        for(auto& board : last_game) {
            if(genomes.count(board) == 0) {
                // If this state is new, initialize all scores randomly
                genomes[board] = {rand() % 100 + 1, rand() % 100 + 1, rand() % 100 + 1, rand() % 100 + 1, rand() % 100 + 1, rand() % 100 + 1, rand() % 100 + 1, rand() % 100 + 1, rand() % 100 + 1};
            }
            
            // Apply the reward/penalty
            int move_index = moves[counter].first * 3 + moves[counter].second;
            genomes[board][move_index] += reward;

            // Set's a floor for negative numbers.
            if (genomes[board][move_index] < 1) {
                genomes[board][move_index] = 1;
            }

            counter++;
        }
    }

    /**
     * @brief Chooses the bot's next move and guarantees it's valid.
     * @param board the current game's board.
     */
    pair<short, short> choose_move(BOARD board) {
        // Randomizes a valid move if this is a new state of the board
        if(genomes.count(board.grid) == 0) {
            vector<pair<short, short>> valid_moves;
            for(short x = 0; x < 3; ++x) 
                for(short y = 0; y < 3; ++y) 
                    if(board.valid_move(x, y)) 
                        valid_moves.push_back({x, y});

            int random_index = rand() % valid_moves.size();
            return valid_moves[random_index];
        }

        // Stores all the valid moves and it's indexes
        vector<pair<pair<short, short>, int>> valid_moves;
        int sum_of_scores = 0;
        for(short x = 0; x < 3; ++x) {
            for(short y = 0; y < 3; ++y) {
                if(board.valid_move(x, y)) {
                    valid_moves.push_back({{x, y}, genomes[board.grid][x*3 + y]});
                    sum_of_scores += genomes[board.grid][x*3 + y];
                }
            }
        }

        // Picks a valid move at random based on a "Roulette Wheel Selection"
        int random_pick = rand() % sum_of_scores;
        int current_sum = 0;
        for(auto& move : valid_moves) {
            current_sum += move.second;
            if(random_pick < current_sum) {
                return move.first;
            }
        }

        // Fallback for edge cases
        return valid_moves.back().first;
    }

    /**
     * @brief Prints the genome for the current board's state before picking a move
     * @param board The game's current board.
     */
    void print_genome(BOARD &board) {
        if(genomes.count(board.grid) == 0){
            cout << "This board state has no records\n";
            return;
        }
        for(auto& genome : genomes[board.grid])
            cout << genome << " ";
        cout << endl;
    }

    /**
     * @brief Clears the bot's history regarding the last game played.
     * This function does not reset the bot's genomes.
     */
    void clear_history(void) {
        last_game.clear();
        moves.clear();
    }
};

/**
 * @class TicTacToeBOT
 * @brief Handles the game's main loop. It allows auto-play to compete againt a bot
 * or have two bots compete against themselves
 */
class TicTacToeBOT {
    private:
    array<BOT, 2> players; // Stores each player's symbols
    bool curr_player; // 0 = p0, 1 = p1
    BOARD board;

    /**
     * @brief Switches the current player.
     */
    void switch_player(void) {
        curr_player = !curr_player; // Toggles 0 and 1
    }

    public:

    TicTacToeBOT() : players{BOT('X'), BOT('O')}, curr_player(0) {}

    /**
     * @brief An auto-player between two bots competing against
     *  each other in a game of tic tac toe.
     * @param print Boolean to turn on console printing of the game. Default: true.
     */
    void botVSbot(bool print = true) {
        // Clears memory
        board.initialize_board();

         // Player's moves (grid index)
        pair<short, short> move = {-1, -1};
        
        // Main game loop
        while(true) {
            if(print)
                board.draw_board();

            // Prints the bot's chance of picking each move
            if(print) {
                cout << "Possible moves: ";
                players[curr_player].print_genome(board);
            }

            // Chooses the next move based on previous games
            if(print)
                cout << "Player " << players[curr_player].symbol << ", make a move (row and column): ";
            move = players[curr_player].choose_move(board); // Guaranteed valid move
            if(print)
                cout << move.first << " " << move.second << endl;
            players[curr_player].register_move(board.grid, move.first, move.second);
            board.make_move(players[curr_player].symbol, move.first, move.second);

            // Stops the game if the current player won
            if(board.check_win(move.first, move.second)) {
                if(print) {
                    board.draw_board();
                    cout << "Player " << players[curr_player].symbol << " won!\n";
                }
                players[curr_player].update_genomes(WIN);
                players[!curr_player].update_genomes(LOSS);
                return;
            }

            // Stops the game if the last move filled up the board (draw);
            if(board.full()) {
                if(print) {
                    board.draw_board();
                    cout << "It's a draw!\n";
                }
                players[curr_player].update_genomes(DRAW);
                players[!curr_player].update_genomes(DRAW);
                return;
            }

            switch_player();
        }
    }
};

int main(void) {
    srand(time(NULL));
    TicTacToeBOT game;
    int i = 3000;
    while(i > 0) {
        i--;
        game.botVSbot();
    }

    return 0;
}