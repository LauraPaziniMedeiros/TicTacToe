#ifndef BOT_H
#define BOT_H

#include "Board.h"
#include <vector>
#include <map>
#include <string>

using namespace std;

class BOT {
    private:
    vector<vector<char>> last_game;
    vector<pair<short, short>> moves;

    vector<char> rotate_grid(const vector<char>& raw_grid);
    vector<char> flip_grid(const vector<char>& grid);
    pair<short, short> rotate_move(const pair<short, short>& raw_move);
    pair<short, short> flip_move(const pair<short, short>& raw_move);
    pair<vector<char>, pair<short, short>> get_canonical(const vector<char>& raw_grid, const pair<short, short>& raw_move, int *rotation, bool *flip);
    pair<vector<char>, pair<short, short>> unget_canonical(const vector<char>& canon_grid, const pair<short, short>& canon_move, const int& rotation, const bool& flip);
    bool canon_valid_move(vector<char> canon, short x, short y);
    vector<long long> raw_genomes(const vector<long long>& canon_genomes, const int& rotation, const bool& flip);

    public:
    map<vector<char>, vector<long long>> genomes;
    char symbol;

    BOT(char symbol = 'X');
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