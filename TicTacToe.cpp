#include "TicTacToe.h"
using namespace std;

/**
 * @brief Class constructor. Initializes all variables.
 */
BOTvsMINIMAX::BOTvsMINIMAX(BOT &b) : TicTacToe(b) {}

/**
 * @brief Runs a game between a bot and an optimal algorithm (minimax)
 * @param print Boolean to turn on console printing of the game. Default = true.
 * @param bot_symbol 'X': The bot starts, 'O': The optimal algorithm starts. 
 * @returns Game's result based on the bot (WIN, LOSS or DRAW).
 */
int BOTvsMINIMAX::run_game(bool print) {
    char minimax_symbol = 'X';
    if(bot.symbol == 'X') {
        minimax_symbol = 'O';
        // curr_player = 0 => BOT
        // curr_player = 1 => MINIMAX
        // 'X' always starts
        curr_player = 0;
    } else curr_player = 1;

    board.reset_board();
    bot.clear_history();

    pair<short, short> move = {-1, -1};

    while(true) {
        if(print)
            board.draw_board();

        if(curr_player == 0) {
            move = bot.choose_move(board);
            // Prints the bot's chance of picking each move
            if(print) {
                cout << "Bot's possible moves: ";
                bot.print_chromossome(board, move);
            }
            board.make_move(bot.symbol, move.first, move.second);
        } else {
            move = Optimal::find_best_move(board, minimax_symbol);
            board.make_move(minimax_symbol, move.first, move.second);
        } 
        cout << "Chosen move: " << move.first << " " << move.second << endl;

        // Stops the game if the current player won
        if(board.check_win(move.first, move.second)) {
            if(curr_player == 0) { // BOT won
                if(print) {
                    board.draw_board();
                    cout << "The bot ('" << bot.symbol << "') won!\n"; 
                }
                bot.update_genome(WIN);
                return WIN;
            }
            else { // BOT lost
                if(print) {
                    board.draw_board();
                    cout << "The optimal algorithm ('" << minimax_symbol << "') won!\n";
                }
                return LOSS;
            }
        }

        // Draw
        if(board.full()) {
            if(print) {
                board.draw_board();
                cout << "It's a draw!\n";
            }
            bot.update_genome(DRAW);
            return DRAW;
        }

        switch_player();
    }
}

/***************************************************************************/

/**
 * @brief Class constructor, initiates the game's variables.
 * @param X BOT class object represented by 'X'
 * @param O BOT class object represented by 'O'
 */
BOTvsBOT::BOTvsBOT(BOT &X, BOT& O) : TicTacToe(X), bot2(O) {
    players = {&bot, &bot2};
}

/**
 * @brief An auto-player between two bots competing against
 *  each other in a game of tic tac toe.
 * @param print Boolean to turn on console printing of the game. Default = true.
 * @return Game's result based on player X (WIN, LOSS or DRAW).
 */
int BOTvsBOT::run_game(bool print) {
    // Clears memory
    board.reset_board();
    players[0]->clear_history();
    players[1]->clear_history();
    players[0]->symbol = 'X';
    players[1]->symbol = 'O';

    // Player's moves (grid index)
    pair<short, short> move = {-1, -1};
    
    // Main game loop
    while(true) {
        if(print)
            board.draw_board();

        // Chooses the next move based on previous games
        move = players[curr_player]->choose_move(board); // Guaranteed valid move
        // Prints the bot's chance of picking each move
        if(print) {
            cout << "Possible moves: ";
            players[curr_player]->print_chromossome(board, move);
        }
        if(print) {
            cout << "Player " << players[curr_player]->symbol << ", make a move (row and column): ";
            cout << move.first << " " << move.second << endl;
        }
            
        board.make_move(players[curr_player]->symbol, move.first, move.second);

        // Stops the game if the current player won
        if(board.check_win(move.first, move.second)) {
            if(print) {
                board.draw_board();
                cout << "Player " << players[curr_player]->symbol << " won!\n";
            }
            players[curr_player]->update_genome(WIN);
            players[!curr_player]->update_genome(LOSS);
            if(players[curr_player]->symbol == 'X') return WIN; else return LOSS;
        }

        // Stops the game if the last move filled up the board (draw);
        if(board.full()) {
            if(print) {
                board.draw_board();
                cout << "It's a draw!\n";
            }
            players[curr_player]->update_genome(DRAW);
            players[!curr_player]->update_genome(DRAW);
            return DRAW;
        }

        switch_player();
    }
}

/***************************************************************************/

/**
 * @brief Class constructor. Initializes all variables.
 */
BOTvsPLAYER::BOTvsPLAYER(BOT& b) : TicTacToe(b), player_symbol('O') {}

/**
 * @brief Runs a game of tic tac toe between a bot and a human.
 * @param print Boolean to turn on console printing of the game. Default = true.
 * @return Game's result based on the bot (WIN, LOSS or DRAW).
 */
int BOTvsPLAYER::run_game(bool print) {
    player_symbol = bot.symbol == 'X' ? 'O' : 'X';
    // curr_player = 0 => BOT
    // curr_player = 1 => PLAYER
    // 'X' always starts
    curr_player = bot.symbol == 'X' ? 0 : 1;

    board.reset_board();
    bot.clear_history();

    pair<short, short> move = {-1, -1};

    while(true) {
        if(print)
            board.draw_board();

        if(curr_player == 0) {
            move = bot.choose_move(board);
            // Prints the bot's chance of picking each move
            if(print) {
                cout << "Bot's possible moves: ";
                bot.print_chromossome(board, move);
                cout << "Chosen move: " << move.first + 1 << " " << move.second + 1 << endl;
            }
            board.make_move(bot.symbol, move.first, move.second);
        } else {
            // Gets player input
            cout << "Make a move (row and column 1-3): ";
            cin >> move.first >> move.second;
            while(!board.valid_move(move.first - 1, move.second - 1)) {
                cout << "Make a move (row and column 1-3): ";
                cin >> move.first >> move.second;
            }
            move.first--;
            move.second--;
            board.make_move(player_symbol, move.first, move.second);
        } 

        // Stops the game if the current player won
        if(board.check_win(move.first, move.second)) {
            if(curr_player == 0) {
                if(print) {
                    board.draw_board();
                    cout << "The bot (" << bot.symbol << ") won!\n"; 
                }
                bot.update_genome(WIN);
                return WIN;
            }
            else {
                if(print) {
                    board.draw_board();
                    cout << "You (" << player_symbol << ") won!\n";
                }
                return LOSS;
            }
        }

        // Draw
        if(board.full()) {
            if(print) {
                board.draw_board();
                cout << "It's a draw!\n";
            }
            bot.update_genome(DRAW);
            return DRAW;
        }

        switch_player();
    }
}