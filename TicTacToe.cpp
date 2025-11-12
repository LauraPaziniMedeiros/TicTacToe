#include <iostream>
using namespace std;

class BOARD {
    private:
    char grid[3][3]; // ' ', 'X' or 'O'

    // Numerical char to short int
    short int to_index(char x) {
        return x - '0';
    }

    bool valid_move(char x, char y) {
        short int x_int = to_index(x);
        short int y_int = to_index(y);

        if(x_int < 0 || x_int > 2)
            return false;
        if(y_int < 0 || y_int > 2)
            return false;
        // Cell already used
        if(grid[x_int][y_int] != ' ')
            return false;

        return true;
    }

    public:
    short int used_cells; // Checks for a draw

    BOARD(void) : used_cells(0) {
        // Creates an empty grid
        for(int i = 0; i < 3; i++)
            for(int j = 0; j < 3; j++)
                grid[i][j] = ' ';
    }

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

    bool full(void) {
        return used_cells == 9;
    }

    bool make_move(char player, char x, char y) {
        if(!valid_move(x, y))
            return false;      
        
        grid[x - '0'][y - '0'] = player;
        used_cells++;
        return true;
    }

    bool is_win(void) {
        // Horizontal win
        int x=0, y=0;
        while(x < 3) {
            if(grid[x][y] != ' ' && grid[x][y] == grid[x][y+1] && grid[x][y] == grid[x][y+2])
                return true;
            x++;
        }

        // Vertical win
        x=0;
        while(y < 3) {
            if(grid[x][y] != ' ' && grid[x][y] == grid[x+1][y] && grid[x][y] == grid[x+2][y])
                return true;
            y++;
        }

        // Diagonal win
        if(grid[0][0] != ' ' && grid[0][0] == grid[1][1] && grid[0][0] == grid[2][2])
            return true;
        if(grid[0][2] != ' ' && grid[0][2] == grid[1][1] && grid[1][1] == grid[2][0])
            return true;

        return false;
    }
};

class TicTacToe {
    private:
    BOARD board;
    char players[2]; // Stores each player's symbols

    public:
    bool curr_player = 0; // 0 = p0, 1 = p1

    TicTacToe(char p0 = 'X', char p1 = 'O') : board(), players{p0, p1}, curr_player(0) {}

    void play(bool print = true) {
        char move[2]; // Player's moves (grid index)
        while(true) {
            if(print)
                board.draw_board();

            // Checks for a draw
            if(board.full()) {
                cout << "It's a draw!\n";
                return;
            }

            // Waits for a valid input move
            if(print)
                cout << "Player " << players[curr_player] << ", make a move (row and collum): ";
            cin >> move[0] >> move[1];
            while(!board.make_move(players[curr_player], move[0], move[1])) {
                if(print) {
                    cout << "Unvalid move.\n";
                    cout << "Player " << players[curr_player] << ", make a move (row and collum): ";
                }
                
                cin >> move[0] >> move[1];
            }

            if(board.is_win()) {
                if(print) {
                    cout << "You won!\n";
                    board.draw_board();
                }
                return;
            }

            // Switch turns
            curr_player = !curr_player;
        }
    }
};

int main() {
    TicTacToe game;
    game.play(false);
    return 0;
}