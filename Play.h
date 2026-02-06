#ifndef PLAY_H
    #define PLAY_H
    #include "Board.h"
    #include "Bot.h"
    #include "Optimal_algorithm.cpp"
    using namespace std;

    /**
     * @class TicTacToeMiniMax
     * @brief Runs a game between a learning bot and an optimal algorithm
     */
    class TicTacToeMiniMax{
        private:
        bool curr_player; // 0 = P1, 1 = P2
        BOARD board;        
        BOT bot;
        Optimal_algorithm minimax;        
        char P1_SYMBOL; // Starting player's symbol
        char P2_SYMBOL; // Second player's symbol

        void switch_player(void);

        public:
        TicTacToeMiniMax();
        short run_game(bool bot_is_x, const bool& print = true);
    };

    /**
     * @class TicTacToeBOT
     * @brief Has two bots compete against themselves.
     */
    class TicTacToeBOT {
        private:
        bool curr_player; // 0 = p0, 1 = p1
        BOARD board;

        void switch_player(void);

        public:
        array<BOT*, 2> players; // Stores each player (BOT)

        TicTacToeBOT(BOT& X, BOT& O);
        short botVSbot(const bool& print = true);
    };

    /**
     * @class TicTacToePlayer
     * @brief Runs a game between a human player and a bot.
     */
    class TicTacToePlayer{
        private:
        bool curr_player; // 0 = P1, 1 = P2
        BOARD board;
        
        BOT* bot_ref;
        
        char P1_SYMBOL; // Símbolo de quem começa
        char P2_SYMBOL; // Símbolo de quem joga em segundo

        void switch_player(void);
    
        public:
        TicTacToePlayer(BOT& bot);
        short run_game(bool bot_is_x, const bool& print = true);
    };

#endif