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
    BOARD rotaciona_90() const{
        BOARD newTab;
        for(int i =0; i<3; i++){
            for(int j =0; j<3; j++){
                newTab.grid[i][j] = grid[2-j][i];
            }
        }
        newTab.used_cells = used_cells;
        return newTab;
    }

    bool equals(const BOARD& other) const{
        return grid == other.grid;
    }

    bool angulo(const BOARD &target) const{
        if(equals(target)) return true;

        BOARD tab90 = this->rotaciona_90();
        if(tab90.equals(target)) return true;

        BOARD tab180 = tab90.rotaciona_90();
        if(tab180.equals(target)) return true;

        BOARD tab270 = tab180.rotaciona_90();
        if(tab270.equals(target)) return true;

        return false;
    }

    char get_cell(int x, int y) const{
        return grid[x][y];
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


    struct Move {
        int row, col;
    };

    // This function returns true if there are moves
    // remaining on the board. It returns false if
    // there are no moves left to play.
    bool isMoveLeft(BOARD& b){
        for(int i = 0; i<3; i++)
            for(int j= 0; j<3;j++)
                if(b.get_cell(i,j)==' ')
                    return true;
        return false;
    }

    /*The evaluate() function examines the current board and returns a numerical score indicating 
    whether the AI is winning, losing, or neither. It checks all rows, columns, and the two diagonals 
    to see if either player has three matching symbols in a line; if the AI has a winning line, it returns +10, 
    and if the opponent has one, it returns –10. If no one is winning, it returns 0, indicating a neutral or 
    ongoing game state. This score guides the Minimax algorithm in choosing the best move.*/
    int evaluate(BOARD &b, char player, char opponent){
        // Checking for Rows for X or O victory.
        for(int row = 0; row<3; row++){
            if(b.get_cell(row, 0) == b.get_cell(row, 1) &&
                b.get_cell(row, 1) == b.get_cell(row, 2))
            {
                if(b.get_cell(row, 0) == player)
                    return +10;
                else if(b.get_cell(row,0) == opponent)
                    return -10;
            }
        }
        // Checking for Columns for X or O victory.
        for(int col = 0; col<3; col++){
            if(b.get_cell(0, col) == b.get_cell(1, col) &&
                b.get_cell(1, col) == b.get_cell(2, col))
            {
                if(b.get_cell(0, col) == player)
                    return +10;
                else if(b.get_cell(0, col) == opponent)
                    return -10;
            }
        }

        // Checking for Diagonals for X or O victory.
        if(b.get_cell(0,0) == b.get_cell(1,1) && 
            b.get_cell(1,1) == b.get_cell(2,2))
        {
            if(b.get_cell(0, 0) == player)
                return +10;
            else if(b.get_cell(0,0) == opponent)
                return -10;
        }

        if(b.get_cell(0,2) == b.get_cell(1,1) && 
            b.get_cell(1,1) == b.get_cell(2,0))
        {
            if(b.get_cell(0,2) == player)
                return +10;
            else if(b.get_cell(0,2) == opponent)
                return -10;
        }
        
        // Else if none of them have won then return 0
        return 0;
    }

    /*This is the minimax function. It considers all
    the possible ways the game can go and returns
    the value of the board*/
    int minimax(BOARD board, int depth, bool isMax, char algorithm, char bot){
        int score = (board, algorithm, bot);
        
        /*If Maximizer has won the game return his/her
        evaluated score*/
        if(score == 10)
            return score - depth;

        /*If Minimizer has won the game return his/her
        evaluated score*/
        if(score == -10)
            return score + depth;
        /*If there are no more moves and no winner then
        it is a tie*/
        if(!isMoveLeft(board))
            return 0;

        // If this maximizer's move
        if(isMax){
            int best = -1000;

            // Traverse all cells
            for(int i = 0; i<3; i++){
                for(int j = 0; j<3; j++){

                    // Check if cell is empty
                    if(board.get_cell(i, j) == ' '){
                        // Make the move
                        BOARD nb = board;
                        nb.make_move(algorithm, i, j);

                         // Call minimax recursively and choose
                        // the maximum value
                        best = max(best, minimax(nb, depth+1, false, algorithm, bot));
                    }
                }
            }
            return best;
        }

        // If this minimizer's move
        else{
            int best = 1000;

            // Traverse all cells
            for(int i = 0; i<3; i++){
                for(int j = 0; j<3; j++){
                     // Check if cell is empty
                    if(board.get_cell(i, j) == ' '){
                        // Make the move
                        BOARD nb = board;
                        nb.make_move(bot, i, j);


                        // Call minimax recursively and choose
                        // the minimum value
                        best = min(best, minimax(nb, depth+1, true, algorithm, bot));
                    }
                }
            }
            return best;
        }
    }

    // This will return the best possible move
    Move findBestMove(char algorithm, char bot){
        int bestVal = -1000;
        Move bestMove = {-1, -1};

        /*Traverse all cells, evaluate minimax function for
        all empty cells. And return the cell with optimal value.*/
        for(int i = 0; i<3; i++){
            for(int j = 0; j<3; j++){
                // Check if cell is empty
                if(board.get_cell(i, j) == ' '){
                    // Make the move
                    BOARD nb = board;
                    nb.make_move(algorithm, i, j);

                    // compute evaluation function for this move.
                    int moveVal = minimax(nb, 0, false, algorithm, bot);

                    /*If the value of the current move is 
                    more than the best value, then update best*/
                    if(moveVal > bestVal){
                        bestVal = moveVal;
                        bestMove = {i, j};
                    }
                }
            }
        }
        return bestMove;
    }

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
            // -------------BOT--------------
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
                return;//const char EMPTY_CELL = ' ';
            }

            //-----------ALGORITHM------------
            else {
                Move best = findBestMove(players[1], players[0]);
                board.make_move(players[1], best.row, best.col);

                if(print) {
                    cout << "AI played (" << best.row << ", " << best.col << ")\n";
                }

                if(board.check_win(best.row, best.col)) {
                    if(print) {
                        board.draw_board();
                        cout << "AI won!\n";
                    }
                    return;
                }
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