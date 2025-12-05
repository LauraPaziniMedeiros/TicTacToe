#ifndef PLAY_H
#define PLAY_H

#include "Board.h"
#include "Bot.h"
#include "Optimal_algorithm.h"
#include <array>

// Classe para Humano vs Bot
class TicTacToeUserVsBot {
    private:
    BOARD board;
    BOT* bot_ref;
    char HUMAN_SYMBOL;
    char BOT_SYMBOL;

    public:
    TicTacToeUserVsBot(BOT& bot);
    void play(bool human_is_x);
};

// Classe para Bot vs Minimax (Treino)
class TicTacToeMiniMax{
    private:
    bool curr_player; 
    BOARD board;
    BOT* bot_ref;
    Optimal_algorithm* minimax_ref;
    char P1_SYMBOL; 
    char P2_SYMBOL; 

    void switch_player(void);

    public:
    TicTacToeMiniMax(BOT& bot, Optimal_algorithm& minimax);
    short run_game(bool bot_is_x, const bool& print = true);
};

// Classe para Bot vs Bot (Treino)
class TicTacToeBOT {
    private:
    bool curr_player;
    BOARD board;
    void switch_player(void);

    public:
    array<BOT, 2> players; // Cópia dos bots para jogar
    // Ajuste: Recebendo ponteiros ou referências seria melhor, mas mantendo compatibilidade
    TicTacToeBOT(BOT& X, BOT& Y);
    short botVSbot(const bool& print = true);
};

#endif