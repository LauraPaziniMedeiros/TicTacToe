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
#include <sstream>
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

    /**
     * @brief Resets the board to it's default settings
     */
    void reset_board(void) {
        for (auto& symbol : grid)
            symbol = EMPTY_CELL;
        used_cells = 0;
    }

    /**
     * @brief Constructs an empty Tic-Tac-Toe board.
     */
    BOARD() : used_cells(0), grid(9) {
        reset_board();
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
    // 'last_game' stores the sequence of board states for this bot in the current game
    vector<vector<char>> last_game;
    // 'moves' stores the {x, y} coordinates for each move in 'last_game'
    vector<pair<short, short>> moves;

    /**
     * @brief Rotates the board 90 degrees clockwise
     * @param raw_grid The original grid (before rotating)
     * @return the new rotated board grid
     */
    vector<char> rotate_grid(const vector<char>& raw_grid) {
        vector<char> new_grid(9);
        for(int i =0; i<3; i++){
            for(int j =0; j<3; j++){
                new_grid[i * 3 + j] = raw_grid[(2-j) * 3 + i];
            }
        }
        return new_grid;
    }

    /**
     * @brief Flips a grid horizontaly
     * @param raw_grid The original grid (before flipping)
     * @return the new flipped grid
     */
    vector<char> flip_grid(const vector<char>& grid) {
        vector<char> flipped = grid;
        for(int r = 0; r < 3; ++r) {
            // Swap col 0 and col 2
            swap(flipped[r*3 + 0], flipped[r*3 + 2]);
        }
        return flipped;
    }

    /**
     * @brief Rotates a move pair 90 degrees clockwise
     * @param raw_grid The original move pair (before rotating)
     * @return the new rotated move pair
     */
    pair<short, short> rotate_move(const pair<short, short>& raw_move) {
        return {raw_move.second, 2 - raw_move.first};
    }

    /**
     * @brief Flips a move horizontaly
     * @param raw_grid The original move pair (before flipping)
     * @return the new flipped move pair
     */
    pair<short, short> flip_move(const pair<short, short>& raw_move) {
        return {raw_move.first, 2 - raw_move.second};
    }

    /**
     * @brief Finds the "Canonical" (standard) form of the board.
     * Checks all 4 rotations and horizontal symmetry and returns the one that is lexicographically smallest.
     * This ensures 0º, 90º , 180º , 270º and symmetrical versions of the same board
     * all map to the same entry in the genomes map.
     * @param raw_grid the original grid
     * @param raw_move pair<x, y> the original move leading to the grid
     * @param rotation the number of 90º rotations to get the canon board
     * @param flip if the canon board was flipped or not
     * @return pair<Canonical Grid, pair<rotated movement>>
     */
    pair<vector<char>, pair<short, short>> get_canonical(
        const vector<char>& raw_grid,
        const pair<short, short>& raw_move, 
        int *rotation,
        bool *flip
    ) {
        vector<char> canon_grid = raw_grid;
        vector<char> curr_grid = raw_grid;
        pair<short, short> canon_move = raw_move;
        pair<short, short> curr_move = raw_move;
        if(rotation != NULL && flip != NULL)
            *rotation = *flip = 0;
        int f = 0, r = 0;

        // Try symmetry
        for(; f <= 1; f++) {
            // Try all rotations
            for(; r <= 3; r++) {
                // Lexicographical comparison
                if (curr_grid < canon_grid) {
                    canon_grid = curr_grid;
                    canon_move = curr_move; 
                    if(rotation != NULL && flip != NULL) {
                        *rotation = r;
                        *flip = f; 
                    }                    
                }
                curr_grid = rotate_grid(curr_grid);
                curr_move = rotate_move(curr_move);
            }
            curr_grid = flip_grid(curr_grid);
            curr_move = flip_move(curr_move);
            r = 0;
        }
        
        return {canon_grid, canon_move};
    }

    /**
     * @brief Returns a canonical grid and move to it's raw input based on the number of
     * rotations and wether it was flipped or not
     * @param canon_grid the canon grid
     * @param canon_move pair<x, y> the canon move leading to the grid
     * @param rotation the number of 90º rotations to get the canon board
     * @param flip if the canon board was flipped or not
     * @return pair<Canonical Grid, pair<rotated movement>>
     */
    pair<vector<char>, pair<short, short>> unget_canonical(
        const vector<char>& canon_grid, 
        const pair<short, short>& canon_move, 
        const int& rotation,
        const bool& flip
    ) {
        vector<char> raw_grid = canon_grid;
        pair<short, short> raw_move = canon_move;

        int rotations = (4 - rotation) % 4;
    
        for(int r = 0; r < rotations; r++) {
            raw_grid = rotate_grid(raw_grid);
            raw_move = rotate_move(raw_move);
        }
        if(flip) {
            raw_grid = flip_grid(raw_grid);
            raw_move = flip_move(raw_move);
        }

        return {raw_grid, raw_move};
    }

    /***
     * @brief Returns if a move is valid given a canon state of the board
     * @param canon the canon board state
     * @param x row
     * @param y column
     */
    bool canon_valid_move(vector<char> canon, short x, short y) {
        // Move is out of bounds
        if(x < 0 || x > 2)
            return false;
        if(y < 0 || y > 2)
            return false;

        // Cell is already used
        if(canon[x*3 + y] != EMPTY_CELL)
            return false;

        return true;
    }

    vector<long long> raw_genomes(const vector<long long>& canon_genomes, const int& rotation, const bool& flip) {
        vector<long long> raw = canon_genomes;
        // rotates
        int rotations = (4 - rotation) % 4;
        for(int i = 0; i < rotations; i++){
            for(int j =0; j<3; j++){
                raw[i * 3 + j] = raw[(2-j) * 3 + i];
            }
        }
        // Flips
        for(int r = 0; r < 3; ++r) {
            // Swap col 0 and col 2
            swap(raw[r*3 + 0], raw[r*3 + 2]);
        }
        return raw;
    }

    public:
    // 'genomes' maps a board state to a vector of 9 scores (one for each cell of the board)
    map<vector<char>, vector<long long>> genomes;
    // The bot's symbol on the board
    char symbol;

    BOT(char symbol = 'X') : symbol(symbol) {}

    /**
     * @brief Clears the bot's history regarding the last game played.
     * This function does not reset the bot's genomes.
     */
    void clear_history(void) {
        last_game.clear();
        moves.clear();
    }

    /**
     * @brief registers the last move made by the bot in it's canonical form.
     * @param grid the board's current grid.
     * @param x the row of the last move.
     * @param y the columm of the last move.
     */
    void register_move(const vector<char>& grid, const short& x, const short& y) {
        auto canon = get_canonical(grid, {x, y}, NULL, NULL);
        last_game.push_back(canon.first);
        moves.push_back(canon.second);
    }

    /***
     * @brief Generates chromossomes for a new board state
     * @param board the current board
     * @return The sum of all the new chromossomes' scores
     */
    int new_board_state(const vector<char>& canon_grid) {
        vector<long long> new_genome(9, 0);
        int sum = 0;
        for(short x = 0; x < 3; x++) 
            for(short y = 0; y < 3; y++) 
                if(canon_valid_move(canon_grid, x, y)) {
                    new_genome[x*3 + y] = 100;
                    sum += 100;
                }
                    
        genomes[canon_grid] = new_genome;
        return sum;
    }

    /**
     * @brief Updates the bot's genomes based on the game's outcome.
     * @param result Represents the result of the game: 
     * 1 if the bot won, -1 if it lost and 0 if it's a draw.
     */
    void update_genomes(const short& result) {
        int counter = 0;
        float reward = 0;

        if(result == WIN) {
            reward = 0.1;
        } else if(result == LOSS) {
            reward = -0.1;
        } else if(result == DRAW) {
            reward = 0.05; // Give a smaller reward for drawing to prefer it over losing
        }

        // Apply reward to all moves made in the game
        for(auto& board : last_game) {
            auto canon = get_canonical(board, moves[counter], NULL, NULL);
            vector<char>& canon_board = canon.first;
            pair<short, short>& canon_move = canon.second;
            short move_index = canon_move.first * 3 + canon_move.second;
            
            // New state of the board
            if(genomes.count(canon_board) == 0)
                new_board_state(canon_board);
            
            // Unvalid move
            if(genomes[canon_board][move_index] == 0) {
                counter++;
                continue;
            }
            
            // Apply the reward/penalty
            long long total = 0;
            for(auto& g : genomes[canon_board])
                total += g;
                
            long long new_chromossome = genomes[canon_board][move_index] + total * reward;
            if(genomes[canon_board][move_index] > 0 && new_chromossome <= 0)
                genomes[canon_board][move_index] = 1;
            else
                genomes[canon_board][move_index] = new_chromossome;

            counter++;
        }
    }

    /**
     * @brief Chooses the bot's next move and guarantees it's valid.
     * @param board the current game's board.
     */
    pair<short, short> choose_move(BOARD board) {
        // Stores the sum of the chromossomes's scores
        int sum_of_scores = 0;
        int rotation;
        bool flip;
        auto canon = get_canonical(board.grid, {0,0}, &rotation, &flip);
        auto& canon_board = canon.first;

        if(genomes.count(canon_board) == 0) { // Creates a new genome
            sum_of_scores = new_board_state(canon_board);
        }
        else {
            for(short x = 0; x < 3; ++x) {
                for(short y = 0; y < 3; ++y) {
                    sum_of_scores += genomes[canon_board][x*3 + y];
                }
            }
        }

        // Picks a valid move at random based on a "Roulette Wheel Selection"
        int random_pick = rand() % sum_of_scores;
        int current_sum = 0;
        int index = 0;
        for(; index < 9; ++index) {
            current_sum += genomes[canon_board][index];
            if(random_pick < current_sum)
                break;
        }
        auto raw = unget_canonical(canon_board, {index / 3, index % 3}, rotation, flip);
        // Registers move
        last_game.push_back(raw.first);
        moves.push_back(raw.second);

        return {raw.second.first, raw.second.second};
    }

    /**
     * @brief Prints the genome for the current board's canonical state before picking a move
     * @param board The game's current board.
     */
    void print_genome(const BOARD &board, const pair<short, short>& move) {
        int rotation;
        bool flip;
        auto canon = get_canonical(board.grid, move, &rotation, &flip);
        if(genomes.count(canon.first) == 0){
            cout << "This board state has no records\n";
            return;
        }

        // Un-rotates the genomes
        auto raw = raw_genomes(genomes[canon.first], rotation, flip);
        for(auto& genome : raw)
            cout << genome << " ";
        cout << endl;
    }

    /**
     * @brief Saves the bot's genomes map to a text file.
     * @param filename The name of the file to save to.
     * @return true if saving was successful, false otherwise.
     */
    bool save_genomes(const string& filename) {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open file for writing: " << filename << endl;
            return false;
        }

        // Iterate through each map entry
        for (const auto& entry : genomes) {
            const vector<char>& board_key = entry.first;
            const vector<long long>& scores = entry.second;

            // Write the board key (9 characters)
            for (int i = 0; i < 9; ++i) {
                // Use a placeholder for the empty cell to avoid file parsing issues
                file << (board_key[i] == EMPTY_CELL ? '_' : board_key[i]);
            }
            
            // Separator
            file << " :"; // Note the space

            // Write the 9 scores
            for (auto& score : scores) {
                file << " " << score;
            }
            file << "\n"; // Newline for the next entry
        }

        file.close();
        return true;
    }

    /**
     * @brief Loads the bot's genomes map from a text file.
     * @param filename The name of the file to load from.
     * @return true if loading was successful, false otherwise.
     */
    bool load_genomes(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            // This is not a critical error if the file just doesn't exist yet
            cout << "Info: Could not open file for reading: " << filename << ". Starting with empty genomes." << endl;
            return false;
        }

        genomes.clear(); // Clear existing genomes before loading
        string line;
        int line_count = 0;

        // Read the file line by line
        while (getline(file, line)) {
            line_count++;
            stringstream ss(line);
            string key_str;
            string separator;
            
            // Read the key part and the separator
            ss >> key_str >> separator;

            if (key_str.length() != 9 || separator != ":") {
                cerr << "Warning: Skipping malformed line " << line_count << ": " << line << endl;
                continue;
            }

            // Convert the key string back to vector<char>
            vector<char> board_key(9);
            for (int i = 0; i < 9; ++i) {
                // Convert placeholder back to empty cell
                board_key[i] = (key_str[i] == '_' ? EMPTY_CELL : key_str[i]);
            }

            // Read the 9 scores
            vector<long long> scores(9);
            bool read_success = true;
            for (int i = 0; i < 9; ++i) {
                if (!(ss >> scores[i])) {
                    read_success = false;
                    break;
                }
            }

            if (!read_success) {
                cerr << "Warning: Skipping malformed scores on line " << line_count << ": " << line << endl;
                continue;
            }

            // Add the entry to the genomes map
            genomes[board_key] = scores;
        }

        file.close();
        return true;
    }
};

/**
 * @class TicTacToeBOT
 * @brief Handles the game's main loop. It allows auto-play to compete againt a bot
 * or have two bots compete against themselves
 */
class TicTacToeBOT {
    private:
    bool curr_player; // 0 = p0, 1 = p1
    BOARD board;

    /**
     * @brief Switches the current player.
     */
    void switch_player(void) {
        curr_player = !curr_player; // Toggles 0 and 1
    }

    public:
    array<BOT, 2> players; // Stores each player (BOT)

    TicTacToeBOT(BOT& X, BOT& Y) : curr_player(0), board(), players{X, Y}{}

    /**
     * @brief An auto-player between two bots competing against
     *  each other in a game of tic tac toe.
     * @param print Boolean to turn on console printing of the game. Default = true.
     * @return Game's result. 0 = draw, 1 = X won, -1 = O won
     */
    short botVSbot(const bool& print = true) {
        // Clears memory
        board.reset_board();

        // Player's moves (grid index)
        pair<short, short> move = {-1, -1};
        // Game's result
        short result = DRAW;
        
        // Main game loop
        while(true) {
            if(print)
                board.draw_board();

            // Chooses the next move based on previous games
            move = players[curr_player].choose_move(board); // Guaranteed valid move
            // Prints the bot's chance of picking each move
            if(print) {
                cout << "Possible moves: ";
                players[curr_player].print_genome(board, move);
            }
            if(print) {
                cout << "Player " << players[curr_player].symbol << ", make a move (row and column): ";
                cout << move.first << " " << move.second << endl;
            }
                
            board.make_move(players[curr_player].symbol, move.first, move.second);

            // Stops the game if the current player won
            if(board.check_win(move.first, move.second)) {
                if(print) {
                    board.draw_board();
                    cout << "Player " << players[curr_player].symbol << " won!\n";
                }
                players[curr_player].update_genomes(WIN);
                players[!curr_player].update_genomes(LOSS);
                if(players[curr_player].symbol == 'X') result = WIN; else result = LOSS;
                break;
            }

            // Stops the game if the last move filled up the board (draw);
            if(board.full()) {
                if(print) {
                    board.draw_board();
                    cout << "It's a draw!\n";
                }
                players[curr_player].update_genomes(DRAW);
                players[!curr_player].update_genomes(DRAW);
                break;
            }

            switch_player();
        }
        return result;
    }
};