#ifndef BOT_H
    #define BOT_H

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
    #include "Board.h"
    using namespace std;

    class BOT {
        private:
        // Stores the sequence of board states for this bot in the current game
        vector<vector<char>> last_game;
        // Stores the {x, y} coordinates for each move in 'last_game'
        vector<pair<short, short>> moves;

        vector<char> rotate_grid(const vector<char>& raw_grid);
        vector<char> flip_grid(const vector<char>& grid);
        pair<short, short> rotate_move(const pair<short, short>& raw_move);
        pair<short, short> flip_move(const pair<short, short>& raw_move);
        pair<vector<char>, pair<short, short>> get_canonical(
            const vector<char>& raw_grid,
            const pair<short, short>& raw_move, 
            int *rotation, bool *flip);
        pair<vector<char>, pair<short, short>> unget_canonical(
            const vector<char>& canon_grid, 
            const pair<short, short>& canon_move, 
            const int& rotation, const bool& flip);
        bool canon_valid_move(vector<char> canon, short x, short y);
        vector<long long> raw_genomes(const vector<long long>& canon_genomes, 
            const int& rotation, const bool& flip);

        public:
        // 'genomes' maps a board state to a vector of 9 scores (one for each cell of the board)
        map<vector<char>, vector<long long>> genomes;
        // The bot's symbol on the board
        char symbol;

        BOT(char symbol = 'X');
        BOT& operator=(const BOT& other);
        void clear_history(void);
        void register_move(const vector<char>& grid, const short& x, const short& y);
        int new_board_state(const vector<char>& canon_grid);
        void update_genomes(const short& result);
        pair<short, short> choose_move(BOARD board);
        void print_genome(const BOARD &board, const pair<short, short>& move);
        bool save_genomes(const string& filename);
        bool load_genomes(const string& filename);
    };

#endif