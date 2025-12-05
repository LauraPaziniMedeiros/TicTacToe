#include "Play.h"
#include <iostream>

using namespace std;

// --- TicTacToeUserVsBot (Humano vs Bot) ---

TicTacToeUserVsBot::TicTacToeUserVsBot(BOT& bot) : board(), bot_ref(&bot) {}

void TicTacToeUserVsBot::play(bool human_is_x) {
    HUMAN_SYMBOL = human_is_x ? 'X' : 'O';
    BOT_SYMBOL = human_is_x ? 'O' : 'X';
    bot_ref->symbol = BOT_SYMBOL;

    board.reset_board();
    bot_ref->clear_history();

    bool current_player_is_human = human_is_x; 
    short result = DRAW;
    
    cout << "\n--- JOGO INICIADO: VOCE (" << HUMAN_SYMBOL << ") vs BOT (" << BOT_SYMBOL << ") ---\n";

    while(true) {
        board.draw_board();
        short x = -1, y = -1;

        if (current_player_is_human) {
            cout << "Sua vez (" << HUMAN_SYMBOL << "). Digite Linha (1-3) e Coluna (1-3): ";
            while(true) {
                cin >> x >> y;
                x --; y--;
                if(board.valid_move(x, y)) break;
                cout << "Movimento invalido! Tente novamente: ";
            }
            board.make_move(HUMAN_SYMBOL, x, y);
        } else {
            cout << "Bot (" << BOT_SYMBOL << ") pensando...\n";
            pair<short, short> move = bot_ref->choose_move(board);
            x = move.first;
            y = move.second;
            board.make_move(BOT_SYMBOL, x, y);
            cout << "Bot jogou em: " << x << " " << y << endl;
        }

        if(board.check_win(x, y)) {
            board.draw_board();
            if(current_player_is_human) {
                cout << "Parabens! Voce venceu!\n";
                result = LOSS; 
            } else {
                cout << "O Bot venceu!\n";
                result = WIN;
            }
            break;
        }
        if(board.full()) {
            board.draw_board();
            cout << "Empate!\n";
            result = DRAW;
            break;
        }
        current_player_is_human = !current_player_is_human;
    }
    bot_ref->update_genomes(result);
}

// --- TicTacToeMiniMax (Bot vs Minimax) ---

void TicTacToeMiniMax::switch_player(void) {
    curr_player = !curr_player; 
}

TicTacToeMiniMax::TicTacToeMiniMax(BOT& bot, Optimal_algorithm& minimax) 
    : curr_player(0), board(), bot_ref(&bot), minimax_ref(&minimax) {}

short TicTacToeMiniMax::run_game(bool bot_is_x, const bool& print) {
    P1_SYMBOL = bot_is_x ? 'X' : 'O'; 
    P2_SYMBOL = bot_is_x ? 'O' : 'X'; 
    
    board.reset_board();
    bot_ref->clear_history();
    curr_player = 0; 

    short result = DRAW;
    pair<short, short> move = {-1, -1};

    while(true) {
        if(print) board.draw_board();

        bool current_player_is_bot = (curr_player == 0 && bot_is_x) || (curr_player == 1 && !bot_is_x);
        char current_symbol = curr_player == 0 ? P1_SYMBOL : P2_SYMBOL;

        if(current_player_is_bot) {
            move = bot_ref->choose_move(board); 
            if(print) {
                cout << "Player " << current_symbol << " (BOT) plays: " << move.first << ", " << move.second << endl;
                bot_ref->print_genome(board, move);
            }
        } else {
            char opponent_symbol = current_symbol == 'X' ? 'O' : 'X';
            Optimal_algorithm::Move minimax_move = 
                minimax_ref->findBestMove(board, current_symbol, opponent_symbol);
            move = {(short)minimax_move.row, (short)minimax_move.col};
            if(print) {
                cout << "Player " << current_symbol << " (Minimax) plays: " << move.first << ", " << move.second << endl;
            }
        }

        board.make_move(current_symbol, move.first, move.second);

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
    bot_ref->update_genomes(result);
    return result;
}

// --- TicTacToeBOT (Bot vs Bot) ---

void TicTacToeBOT::switch_player(void) {
    curr_player = !curr_player; 
}

TicTacToeBOT::TicTacToeBOT(BOT& X, BOT& Y) : curr_player(0), board(), players{X, Y}{}

short TicTacToeBOT::botVSbot(const bool& print) {
    board.reset_board();
    pair<short, short> move = {-1, -1};
    short result = DRAW;
    
    while(true) {
        if(print) board.draw_board();

        move = players[curr_player].choose_move(board); 
        if(print) {
            cout << "Possible moves: ";
            players[curr_player].print_genome(board, move);
            cout << "Player " << players[curr_player].symbol << ", make a move (row and column): ";
            cout << move.first << " " << move.second << endl;
        }
            
        board.make_move(players[curr_player].symbol, move.first, move.second);

        if(board.check_win(move.first, move.second)) {
            if(print) {
                board.draw_board();
                cout << "Player " << players[curr_player].symbol << " won!\n";
            }
            players[curr_player].update_genomes(WIN);
            players[!curr_player].update_genomes(LOSS);
            if(players[curr_player].symbol == 'X') result = WIN; else result = LOSS;
            break;
        }

        if(board.full()) {
            if(print) {
                board.draw_board();
                cout << "It's a draw!\n";
            }
            players[curr_player].update_genomes(DRAW);
            players[!curr_player].update_genomes(DRAW);
            break;
        }
        switch_player();
    }
    return result;
}