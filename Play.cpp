
#include "Board.h"
#include "Bot.cpp"
#include "Optimal_algorithm.cpp"

class TicTacToeMiniMax{
    private:
    bool curr_player; // 0 = P1, 1 = P2
    BOARD board;
    
    BOT* bot_ref;
    Optimal_algorithm* minimax_ref;
    
    char P1_SYMBOL; // Símbolo de quem começa
    char P2_SYMBOL; // Símbolo de quem joga em segundo

    void switch_player(void) {
        curr_player = !curr_player; 
    }

    public:
    // Construtor: Recebe o BOT e o Minimax por referência.
    TicTacToeMiniMax(BOT& bot, Optimal_algorithm& minimax) 
        : curr_player(0), board(), bot_ref(&bot), minimax_ref(&minimax) {}

    /**
     * @brief Roda um jogo onde o BOT pode ser P1 ('X') ou P2 ('O') contra o Minimax.
     * @param bot_is_x: Se TRUE, BOT é 'X' e Minimax é 'O'. Se FALSE, BOT é 'O' e Minimax é 'X'.
     * @return short: O resultado do jogo (WIN, LOSS, DRAW) para o BOT evolutivo.
     */
    short run_game(bool bot_is_x, const bool& print = true) {
        P1_SYMBOL = bot_is_x ? 'X' : 'O'; // P1: BOT ou Minimax
        P2_SYMBOL = bot_is_x ? 'O' : 'X'; // P2: Minimax ou BOT
        
        board.reset_board();
        bot_ref->clear_history();
        curr_player = 0; // P1 sempre começa

        short result = DRAW;
        pair<short, short> move = {-1, -1};

        while(true) {
            if(print) board.draw_board();

            // Determina quem joga e qual é o símbolo
            bool current_player_is_bot = (curr_player == 0 && bot_is_x) || (curr_player == 1 && !bot_is_x);
            char current_symbol = curr_player == 0 ? P1_SYMBOL : P2_SYMBOL;

            if(current_player_is_bot) {
                // Turnto do BOT de Aprendizado
                move = bot_ref->choose_move(board); 
                
                if(print) {
                    cout << "Player " << current_symbol << " (BOT) plays: " << move.first << ", " << move.second << endl;
                    cout << "Possible moves (Chromosomes): ";
                    // Chamada a print_genome deve usar o BOT, que é referenciado por bot_ref
                    bot_ref->print_genome(board, move);
                }
            } else {
                // Turnto do MINIMAX
                // O Minimax precisa saber seu símbolo atual (current_symbol) e o do oponente.
                char opponent_symbol = current_symbol == 'X' ? 'O' : 'X';
                Optimal_algorithm::Move minimax_move = 
                    minimax_ref->findBestMove(board, current_symbol, opponent_symbol);
                
                move = {(short)minimax_move.row, (short)minimax_move.col};

                if(print) {
                    cout << "Player " << current_symbol << " (Minimax) plays: " << move.first << ", " << move.second << endl;
                }
            }

            board.make_move(current_symbol, move.first, move.second);

            // Checagem de vitória/empate
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
        
        // APRENDIZADO DO BOT: O genoma do objeto original pop[i].first é atualizado.
        bot_ref->update_genomes(result);
        return result;
    }

};

/**
 * @class TicTacToeBOT
 * @brief Handles the game's main loop. It allows auto-play to compete againt a bot
 * or have two bots compete against themselves
 */
class TicTacToeBOT {
    private:
    bool curr_player; // 0 = p0, 1 = p1
    BOARD board;

    /**
     * @brief Switches the current player.
     */
    void switch_player(void) {
        curr_player = !curr_player; // Toggles 0 and 1
    }

    public:
    array<BOT, 2> players; // Stores each player (BOT)

    TicTacToeBOT(BOT& X, BOT& Y) : curr_player(0), board(), players{X, Y}{}

    /**
     * @brief An auto-player between two bots competing against
     *  each other in a game of tic tac toe.
     * @param print Boolean to turn on console printing of the game. Default = true.
     * @return Game's result. 0 = draw, 1 = X won, -1 = O won
     */
    short botVSbot(const bool& print = true) {
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
            move = players[curr_player].choose_move(board); // Guaranteed valid move
            // Prints the bot's chance of picking each move
            if(print) {
                cout << "Possible moves: ";
                players[curr_player].print_genome(board, move);
            }
            if(print) {
                cout << "Player " << players[curr_player].symbol << ", make a move (row and column): ";
                cout << move.first << " " << move.second << endl;
            }
                
            board.make_move(players[curr_player].symbol, move.first, move.second);

            // Stops the game if the current player won
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

            // Stops the game if the last move filled up the board (draw);
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
};