#include "Play.h"
using namespace std;

/* BOT vs Optimal Algorithm: TicTacToeMiniMax class */

/**
 * @brief Switches the current player variable.
 */
void TicTacToeMiniMax::switch_player(void) {
    curr_player = !curr_player; 
}

/**
 * @brief Class constructor, initializes the game's variables.
 */
TicTacToeMiniMax::TicTacToeMiniMax() 
    : curr_player(0), board(), bot(), minimax() {}

/**
 * @brief Runs a game between a BOT and an optimal algorithm.
 * @param bot_is_x TRUE: the BOT is represented by 'X' on the board. 
 * FALSE: the bot is represented by 'O' on the board.
 * @return The BOT's result in this game (WIN, LOSS or DRAW).
 */
short TicTacToeMiniMax::run_game(bool bot_is_x, const bool& print) {
    P1_SYMBOL = bot_is_x ? 'X' : 'O'; // P1: BOT or Minimax
    P2_SYMBOL = bot_is_x ? 'O' : 'X'; // P2: Minimax or BOT
    
    board.reset_board();
    bot.clear_history();
    curr_player = 0; // P1 always starts

    short result = DRAW;
    pair<short, short> move = {-1, -1};

    while(true) {
        if(print) board.draw_board();

        // Determines who plays and which symbol they represent
        bool current_player_is_bot = (curr_player == 0 && bot_is_x) || (curr_player == 1 && !bot_is_x);
        char current_symbol = curr_player == 0 ? P1_SYMBOL : P2_SYMBOL;

        if(current_player_is_bot) {
            move = bot.choose_move(board); 
            
            if(print) {
                cout << "Player " << current_symbol << " (BOT) plays: " << move.first << ", " << move.second << endl;
                cout << "Possible moves (Chromosomes): ";
                bot.print_chromossome(board, move);
            }
        } else {
            char opponent_symbol = current_symbol == 'X' ? 'O' : 'X';
            Optimal_algorithm::Move minimax_move = 
                minimax.findBestMove(board, current_symbol, opponent_symbol);
            
            move = {(short)minimax_move.row, (short)minimax_move.col};

            if(print) {
                cout << "Player " << current_symbol << " (Minimax) plays: " << move.first << ", " << move.second << endl;
            }
        }

        board.make_move(current_symbol, move.first, move.second);

        // Win check
        if(board.check_win(move.first, move.second)) {
            result = current_player_is_bot ? WIN : LOSS;
            if(print) { board.draw_board(); cout << "Player " << current_symbol << " won!\n"; }
            break;
        }

        if(board.full()) {
            result = DRAW;
            if(print) { board.draw_board(); cout << "It's a draw!\n"; }
            break;
        }

        switch_player();
    }
    
    // Updates de bot's genome
    bot.update_genome(result);
    return result;
}

/* BOT vs BOT: TicTacToeBOT class */

/**
 * @brief Switches the current player.
 */
void TicTacToeBOT::switch_player(void) {
    curr_player = !curr_player; // Toggles 0 and 1
}

/**
 * @brief Class constructor, initiates the game's variables.
 * @param X BOT class object represented by 'X'
 * @param O BOT class object represented by 'O'
 */
TicTacToeBOT::TicTacToeBOT(BOT& X, BOT& O) : curr_player(0), board(), players{&X, &O}{}

/**
 * @brief An auto-player between two bots competing against
 *  each other in a game of tic tac toe.
 * @param print Boolean to turn on console printing of the game. Default = true.
 * @return Game's result based on player 1 (WIN, LOSS or DRAW).
 */
short TicTacToeBOT::botVSbot(const bool& print) {
    // Clears memory
    board.reset_board();

    // Player's moves (grid index)
    pair<short, short> move = {-1, -1};
    // Game's result
    short result = DRAW;
    
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
            if(players[curr_player]->symbol == 'X') result = WIN; else result = LOSS;
            break;
        }

        // Stops the game if the last move filled up the board (draw);
        if(board.full()) {
            if(print) {
                board.draw_board();
                cout << "It's a draw!\n";
            }
            players[curr_player]->update_genome(DRAW);
            players[!curr_player]->update_genome(DRAW);
            break;
        }

        switch_player();
    }
    return result;
}

/* PLAYER vs BOT: TicTacToePlayer class */

/**
 * @brief Switches the current player variable.
 */
void TicTacToePlayer::switch_player(void) {
    curr_player = !curr_player; 
}

/**
 * @brief Class constructor, initializes the game's variables for a new game.
 */
TicTacToePlayer::TicTacToePlayer(BOT& bot) 
    : curr_player(0), board(), bot_ref(&bot) {}

/**
 * @brief Runs a game where the bot can be P1 ('X') or P2 ('O') against a human player.
 * @param bot_is_x: If TRUE, the bot is represented by 'X' on the board, if FALSE, by 'O'.
 * @return The BOT's result in this game (WIN, LOSS or DRAW).
 */
short TicTacToePlayer::run_game(bool bot_is_x, const bool& print) {
    P1_SYMBOL = bot_is_x ? 'X' : 'O'; // P1: BOT or player
    P2_SYMBOL = bot_is_x ? 'O' : 'X'; // P2: player or BOT
    
    board.reset_board();
    bot_ref->clear_history();
    curr_player = 0; // P1 always starts

    bot_ref->symbol = bot_is_x ? 'X' : 'O';

    short result = DRAW;
    pair<short, short> move = {-1, -1};

    while(true) {
        if(print) board.draw_board();

        // Determines who plays and their symbol
        bool current_player_is_bot = (curr_player == 0 && bot_is_x) || (curr_player == 1 && !bot_is_x);
        char current_symbol = curr_player == 0 ? P1_SYMBOL : P2_SYMBOL;

        if(current_player_is_bot) {
            move = bot_ref->choose_move(board); 
            
            if(print) {
                cout << "Player " << current_symbol << " (BOT) plays: " << move.first << ", " << move.second << endl;
                cout << "Possible moves (Chromosomes): ";
                bot_ref->print_chromossome(board, move);
            }
        } else {
            int x = -1, y = -1;
            while(!board.valid_move(x, y)) {
                cout << "Choose a valid row (1-3) and a collumn (1-3)";
                cin >> x >> y;
                x --; y --;
                
                move = {x, y};
            }
            
            if(print) {
                cout << "Player " << current_symbol << " (User) plays: " << move.first << ", " << move.second << endl;
            }
        }

        board.make_move(current_symbol, move.first, move.second);

        // Win check
        if(board.check_win(move.first, move.second)) {
            result = current_player_is_bot ? WIN : LOSS;
            if(print) { board.draw_board(); cout << "Player " << current_symbol << " won!\n"; }
            break;
        }

        if(board.full()) {
            result = DRAW;
            if(print) { board.draw_board(); cout << "It's a draw!\n"; }
            break;
        }

        switch_player();
    }
    
    // Updates de bot's genome
    bot_ref->update_genome(result);
    return result;
}