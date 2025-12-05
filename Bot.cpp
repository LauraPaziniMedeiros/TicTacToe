#include "Bot.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <random>
#include <fstream>
#include <sstream>

BOT::BOT(char symbol) : symbol(symbol) {}

vector<char> BOT::rotate_grid(const vector<char>& raw_grid) {
    vector<char> new_grid(9);
    for(int i =0; i<3; i++){
        for(int j =0; j<3; j++){
            new_grid[i * 3 + j] = raw_grid[(2-j) * 3 + i];
        }
    }
    return new_grid;
}

vector<char> BOT::flip_grid(const vector<char>& grid) {
    vector<char> flipped = grid;
    for(int r = 0; r < 3; ++r) {
        swap(flipped[r*3 + 0], flipped[r*3 + 2]);
    }
    return flipped;
}

pair<short, short> BOT::rotate_move(const pair<short, short>& raw_move) {
    return {raw_move.second, 2 - raw_move.first};
}

pair<short, short> BOT::flip_move(const pair<short, short>& raw_move) {
    return {raw_move.first, 2 - raw_move.second};
}

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

    for(; f <= 1; f++) {
        for(; r <= 3; r++) {
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

bool BOT::canon_valid_move(vector<char> canon, short x, short y) {
    if(x < 0 || x > 2 || y < 0 || y > 2) return false;
    if(canon[x*3 + y] != EMPTY_CELL) return false;
    return true;
}

vector<long long> BOT::raw_genomes(const vector<long long>& canon_genomes, const int& rotation, const bool& flip) {
    vector<long long> raw = canon_genomes;
    int rotations = (4 - rotation) % 4;
    for(int i = 0; i < rotations; i++){
        for(int j =0; j<3; j++){
            raw[i * 3 + j] = raw[(2-j) * 3 + i];
        }
    }
    for(int r = 0; r < 3; ++r) {
        swap(raw[r*3 + 0], raw[r*3 + 2]);
    }
    return raw;
}

void BOT::clear_history(void) {
    last_game.clear();
    moves.clear();
}

void BOT::register_move(const vector<char>& grid, const short& x, const short& y) {
    auto canon = get_canonical(grid, {x, y}, NULL, NULL);
    last_game.push_back(canon.first);
    moves.push_back(canon.second);
}

int BOT::new_board_state(const vector<char>& canon_grid) {
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

void BOT::update_genomes(const short& result) {
    int counter = 0;
    float reward = 0;

    if(result == WIN) {
        reward = 0.2;
    } else if(result == LOSS) {
        reward = -0.05;
    } else if(result == DRAW) {
        reward = 0.1;
    }

    for(auto& board : last_game) {
        auto canon = get_canonical(board, moves[counter], NULL, NULL);
        vector<char>& canon_board = canon.first;
        pair<short, short>& canon_move = canon.second;
        short move_index = canon_move.first * 3 + canon_move.second;
        
        if(genomes.count(canon_board) == 0)
            new_board_state(canon_board);
        
        if(genomes[canon_board][move_index] == 0) {
            counter++;
            continue;
        }
        
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

pair<short, short> BOT::choose_move(BOARD board) {
    int sum_of_scores = 0;
    int rotation;
    bool flip;
    auto canon = get_canonical(board.grid, {0,0}, &rotation, &flip);
    auto& canon_board = canon.first;

    if(genomes.count(canon_board) == 0) {
        sum_of_scores = new_board_state(canon_board);
    }
    else {
        for(short x = 0; x < 3; ++x) {
            for(short y = 0; y < 3; ++y) {
                sum_of_scores += genomes[canon_board][x*3 + y];
            }
        }
    }

    if(sum_of_scores == 0){
        if(board.isMoveLeft()){
            sum_of_scores = 1;
        } else {
            return{-1,-1};
        }
    }

    int random_pick = rand() % sum_of_scores;
    int current_sum = 0;
    int index = 0;
    for(; index < 9; ++index) {
        current_sum += genomes[canon_board][index];
        if(random_pick < current_sum)
            break;
    }
    auto raw = unget_canonical(canon_board, {index / 3, index % 3}, rotation, flip);
    last_game.push_back(raw.first);
    moves.push_back(raw.second);

    return {raw.second.first, raw.second.second};
}

void BOT::print_genome(const BOARD &board, const pair<short, short>& move) {
    int rotation;
    bool flip;
    auto canon = get_canonical(board.grid, move, &rotation, &flip);
    if(genomes.count(canon.first) == 0){
        cout << "This board state has no records\n";
        return;
    }
    auto raw = raw_genomes(genomes[canon.first], rotation, flip);
    for(auto& genome : raw)
        cout << genome << " ";
    cout << endl;
}

bool BOT::save_genomes(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file for writing: " << filename << endl;
        return false;
    }
    for (const auto& entry : genomes) {
        const vector<char>& board_key = entry.first;
        const vector<long long>& scores = entry.second;
        for (int i = 0; i < 9; ++i) {
            file << (board_key[i] == EMPTY_CELL ? '_' : board_key[i]);
        }
        file << " :";
        for (auto& score : scores) {
            file << " " << score;
        }
        file << "\n";
    }
    file.close();
    return true;
}

bool BOT::load_genomes(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Info: Could not open file for reading: " << filename << ". Starting with empty genomes." << endl;
        return false;
    }
    genomes.clear(); 
    string line;
    int line_count = 0;
    while (getline(file, line)) {
        line_count++;
        stringstream ss(line);
        string key_str;
        string separator;
        
        ss >> key_str >> separator;
        if (key_str.length() != 9 || separator != ":") continue;

        vector<char> board_key(9);
        for (int i = 0; i < 9; ++i) {
            board_key[i] = (key_str[i] == '_' ? EMPTY_CELL : key_str[i]);
        }
        vector<long long> scores(9);
        bool read_success = true;
        for (int i = 0; i < 9; ++i) {
            if (!(ss >> scores[i])) {
                read_success = false;
                break;
            }
        }
        if (!read_success) continue;
        genomes[board_key] = scores;
    }
    file.close();
    return true;
}