#ifndef BOARD_H
    #define BOARD_H

    #include <vector>
    #include <iostream>
    #include <map>
    #include <string>

    using namespace std;

    // --- CONSTANTS ---
    const char EMPTY_CELL = ' '; // Defines an empty cell for the game board
    #define WIN 1
    #define LOSS -1
    #define DRAW 0

    /**
     * @class BOARD
     * @brief Manages the 3x3 grid state for a Tic-Tac-Toe game.
     */
    class BOARD {
    public:
        short int used_cells;
        vector<char> grid;

        BOARD();
        void reset_board(void);        
        bool valid_move(short int x, short int y) const;
        void draw_board(void);
        bool move_available(void);
        bool full(void);
        bool make_move(char player, short int x, short int y);
        bool check_win(short int x, short int y);
        char get_cell(short int x, short int y) const;
    };

#endif // BOARD_H