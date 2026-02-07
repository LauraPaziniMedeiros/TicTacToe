#ifndef BOT_H
    #define BOT_H

    #include <array>
    #include <stdlib.h>
    #include <time.h>
    #include <algorithm>
    #include <random>
    #include <fstream>
    #include <sstream>
    #include "Board.h"
    #include "Random64.h"
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
        vector<unsigned long long> raw_chromossome(const vector<unsigned long long>& canon_chrom, 
            const int& rotation, const bool& flip);

        public:
        // 'genome' maps every board state to a vector of 9 scores (one for each cell of the board)
        map<vector<char>, vector<unsigned long long>> genome;
        // The bot's symbol on the board
        char symbol;

        BOT(char symbol = 'X');
        BOT& operator=(const BOT& other);
        void clear_history(void);
        void register_move(const vector<char>& grid, const short& x, const short& y);
        int new_chromossome(const vector<char>& canon_grid);
        void update_genome(const short& result);
        pair<short, short> choose_move(const BOARD& board);
        void print_chromossome(const BOARD& board, const pair<short, short>& move);
        bool save_genome(const string& filename);
        bool load_genome(const string& filename);
    };

#endif