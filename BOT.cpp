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
    vector<char> rotate_90(vector<char> raw_grid) {
        vector<char> new_grid(9);
        for(int i =0; i<3; i++){
            for(int j =0; j<3; j++){
                new_grid[i * 3 + j] = raw_grid[(2-j) * 3 + i];
            }
        }
        return new_grid;
    }

    /**
     * @brief Finds the "Canonical" (standard) form of the board.
     * Checks all 4 rotations and returns the one that is lexicographically smallest.
     * This ensures 0 deg, 90 deg, 180 deg, and 270 deg versions of the same board
     * all map to the SAME entry in the genomes map.
     * @param raw_grid the original grid
     * @param raw_move pair<x, y> the original move leading to the grid
     * @return pair<Canonical Grid, pair<rotated movement>>
     */
    pair<vector<char>, pair<short, short>> get_canonical(const vector<char>& raw_grid, const pair<short, short>& raw_move) {
        vector<char> canon_grid = raw_grid;
        pair<short, short> canon_move = raw_move;
        // Try all rotations
        for(int i = 0; i < 3; i++) {
            vector<char> rotated = rotate_90(raw_grid);
            
            // Lexicographical comparison
            if (rotated < canon_grid) {
                canon_grid = rotated;
                canon_move = {2 - raw_move.second, raw_move.first};
            }
        }
        return {canon_grid, canon_move};
    }

    public:
    // 'genomes' maps a board state to a vector of 9 scores (one for each cell of the board)
    map<vector<char>, vector<int>> genomes;
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
        auto canon = get_canonical(grid, {x, y});
        last_game.push_back(canon.first);
        moves.push_back(canon.second);
    }

    /**
     * @brief Updates the bot's "genome" based on the game's outcome.
     * @param result Represents the result of the game: 
     * 1 if the bot won, -1 if it lost and 0 if it's a draw.
     */
    void update_genomes(const short& result) {
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
            auto canon = get_canonical(board, moves[counter]);
            vector<char>& canon_board = canon.first;
            pair<short, short> canon_move = canon.second;
            if(genomes.count(canon.first) == 0) {
                // If this state is new, initialize all scores randomly
                genomes[canon_board] = {rand() % 10 + 1, rand() % 10 + 1, rand() % 10 + 1, rand() % 10 + 1, rand() % 10 + 1, rand() % 10 + 1, rand() % 10 + 1, rand() % 10 + 1, rand() % 10 + 1};
            }
            
            // Apply the reward/penalty
            short move_index = canon_move.first * 3 + canon_move.second;
            genomes[canon_board][move_index] += reward;

            // Set's a floor for negative numbers.
            if (genomes[canon_board][move_index] < 1) {
                genomes[canon_board][move_index] = 1;
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
     * @brief Prints the genome for the current board's canonical state before picking a move
     * @param board The game's current board.
     */
    void print_genome(const BOARD &board, pair<short, short>& move) {
        auto canon = get_canonical(board.grid, move);
        if(genomes.count(canon.first) == 0){
            cout << "This board state has no records\n";
            return;
        }
        for(auto& genome : genomes[canon.first])
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
            const vector<int>& scores = entry.second;

            // Write the board key (9 characters)
            for (int i = 0; i < 9; ++i) {
                // Use a placeholder for the empty cell to avoid file parsing issues
                file << (board_key[i] == EMPTY_CELL ? '_' : board_key[i]);
            }
            
            // Separator
            file << " :"; // Note the space

            // Write the 9 scores
            for (int score : scores) {
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
            vector<int> scores(9);
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