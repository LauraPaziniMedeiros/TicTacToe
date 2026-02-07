#include "Bot.h"
using namespace std;

/**
 * @brief Initiates a BOT with a given symbol.
 * @param symbol The bot's symbol in the gameboard.
 */
BOT::BOT(char symbol) : symbol(symbol) {}

BOT& BOT::operator=(const BOT& other) {
    this->last_game = other.last_game;
    this->moves = other.moves;
    this->genome = other.genome;
    this->symbol = other.symbol;
    return *this;
}

/**
 * @brief Rotates the board 90 degrees clockwise
 * @param raw_grid The original grid (before rotating)
 * @return the new rotated board grid
 */
vector<char> BOT::rotate_grid(const vector<char>& raw_grid) {
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
vector<char> BOT::flip_grid(const vector<char>& grid) {
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
pair<short, short> BOT::rotate_move(const pair<short, short>& raw_move) {
    return {raw_move.second, 2 - raw_move.first};
}

/**
 * @brief Flips a move horizontaly
 * @param raw_grid The original move pair (before flipping)
 * @return the new flipped move pair
 */
pair<short, short> BOT::flip_move(const pair<short, short>& raw_move) {
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
pair<vector<char>, pair<short, short>> BOT::get_canonical(
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
pair<vector<char>, pair<short, short>> BOT::unget_canonical(
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
bool BOT::canon_valid_move(vector<char> canon, short x, short y) {
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

/***
 * @brief Un-rotates and un-flips a canon chromossome.
 * @param canon_chrom the rotated/flipped chromossome
 * @param rotation the number of 90º rotations
 * @param flip if the chromossome was flipped or not
 * @return The raw chromossome.
 */
vector<unsigned long long> BOT::raw_chromossome(const vector<unsigned long long>& canon_chrom, 
    const int& rotation, const bool& flip
) {
    vector<unsigned long long> raw = canon_chrom;
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

/**
 * @brief Clears the bot's history regarding the last game played.
 * This function does not reset the bot's genome.
 */
void BOT::clear_history(void) {
    last_game.clear();
    moves.clear();
}

/**
 * @brief registers the last move made by the bot in it's canonical form.
 * @param grid the board's current grid.
 * @param x the row of the last move.
 * @param y the columm of the last move.
 */
void BOT::register_move(const vector<char>& grid, const short& x, const short& y) {
    auto canon = get_canonical(grid, {x, y}, NULL, NULL);
    last_game.push_back(canon.first);
    moves.push_back(canon.second);
}

/***
 * @brief Generates chromossomes for a new board state
 * @param board the canon (flipped and rotated) board
 * @return The sum of all the new chromossomes' scores
 */
int BOT::new_chromossome(const vector<char>& canon_grid) {
    vector<unsigned long long> new_chrom(9, 0);
    int sum = 0; // Already sums the genes for the roulette wheel selection
    for(short x = 0; x < 3; x++) 
        for(short y = 0; y < 3; y++) 
            if(canon_valid_move(canon_grid, x, y)) {
                new_chrom[x*3 + y] = 100;
                sum += 100;
            }

    genome[canon_grid] = new_chrom;
    return sum;
}

/**
 * @brief Updates the bot's genome based on the last game's outcome.
 * @param result Represents the result of the game: 
 * 1 if the bot won, -1 if it lost and 0 if it's a draw.
 */
void BOT::update_genome(const short& result) {
    int counter = 0;
    float reward = 0;

    if(result == WIN) {
        reward = 0.2;
    } else if(result == LOSS) {
        reward = -0.05;
    } else if(result == DRAW) {
        reward = 0.1; // Give a smaller reward for drawing to prefer it over losing
    }

    // Apply reward to all moves made in the game
    for(auto& board : last_game) {
        auto canon = get_canonical(board, moves[counter], NULL, NULL);
        vector<char>& canon_board = canon.first;
        pair<short, short>& canon_move = canon.second;
        short move_index = canon_move.first * 3 + canon_move.second;

        // New state of the board
        if(genome.count(canon_board) == 0)
            new_chromossome(canon_board);
        
        // Unvalid move
        if(genome[canon_board][move_index] == 0) {
            counter++;
            continue;
        }
        
        // Apply the reward/penalty
        unsigned long long total = 0;
        for(auto& gene : genome[canon_board])
            total += gene;
            
        unsigned long long new_gene = genome[canon_board][move_index] + total * reward;
        if(new_gene <= 0) {
            // A valid move should remain available for playing
            if(genome[canon_board][move_index] > 0)
                genome[canon_board][move_index] = 1;
            else 
                genome[canon_board][move_index] = 0;
        } else genome[canon_board][move_index] = new_gene;

        counter++;
    }
}

/**
 * @brief Chooses the bot's next move and guarantees it's valid.
 * @param board the current game's board.
 * @returns a pair with the coordinates of the bot's next move.
 */
pair<short, short> BOT::choose_move(const BOARD& board) {
    // Stores the sum of the chromossomes's scores
    unsigned long long sum_of_scores = 0;
    int rotation;
    bool flip;
    auto canon = get_canonical(board.grid, {0,0}, &rotation, &flip);
    auto& canon_board = canon.first;

    if(genome.count(canon_board) == 0) { // Creates a new chromossome
        sum_of_scores = new_chromossome(canon_board);
    } else {
        for(short x = 0; x < 3; ++x) {
            for(short y = 0; y < 3; ++y) {
                sum_of_scores += genome[canon_board][x*3 + y];
            }
        }
    }

    // Picks a valid move at random based on a "Roulette Wheel Selection"
    unsigned long long current_sum = 0;
    unsigned long long random_pick = Random64::range(0, sum_of_scores - 1);
    int index = 0;
    for(; index < 9; ++index) {
        current_sum += genome[canon_board][index];
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
 * @param move The last move leading up to this board state.
 */
void BOT::print_chromossome(const BOARD &board, const pair<short, short>& move) {
    int rotation;
    bool flip;
    auto canon = get_canonical(board.grid, move, &rotation, &flip);
    if(genome.count(canon.first) == 0){
        cout << "This board state has no records\n";
        return;
    }

    // Un-rotates the chromossome to match the board printed onto the console
    auto raw = raw_chromossome(genome[canon.first], rotation, flip);
    for(auto& genome : raw)
        cout << genome << " ";
    cout << endl;
}

/**
 * @brief Saves the bot's genome map to a text file.
 * @param filename The name of the file to save to.
 * @return true if saving was successful, false otherwise.
 */
bool BOT::save_genome(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file for writing: " << filename << endl;
        return false;
    }

    // Iterate through each map entry
    for (const auto& entry : genome) {
        const vector<char>& board_key = entry.first;
        const vector<unsigned long long>& scores = entry.second;

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
 * @brief Loads the bot's genome map from a text file.
 * @param filename The name of the file to load from.
 * @return true if loading was successful, false otherwise.
 */
bool BOT::load_genome(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        // This is not a critical error if the file just doesn't exist yet
        cout << "Info: Could not open file for reading: " << filename << ". Starting with empty genomes." << endl;
        return false;
    }

    genome.clear(); // Clear existing genomes before loading
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
        vector<unsigned long long> scores(9);
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
        genome[board_key] = scores;
    }

    file.close();
    return true;
}