#ifndef TICTACTOE_H
    #define TICTACTOE_H
    #include "OptimalAlgorithm.h"

    class TicTacToe {
        protected:
        BOARD board;
        BOT& bot;
        bool curr_player;

        void switch_player(void) {curr_player = !curr_player;}
        TicTacToe(BOT &b) : board(), bot(b), curr_player(0) {}
        virtual ~TicTacToe() {}

        public:
        void set_bot_symbol(const char& symbol) {bot.symbol = symbol;}
        virtual int run_game(bool print = true) = 0;
    };

    class BOTvsMINIMAX : public TicTacToe {
        public:
        BOTvsMINIMAX(BOT &b);
        int run_game(bool print = true) override;
    };

    class BOTvsBOT : public TicTacToe {
        private:
        BOT& bot2;
        array<BOT*, 2> players;

        public:
        BOTvsBOT(BOT &X, BOT& O);
        int run_game(bool print = true) override;
    };

    class BOTvsPLAYER : public TicTacToe {
        private:
        char player_symbol;
        
        public:
        BOTvsPLAYER(BOT& b);
        int run_game(bool print = true) override;
    };
#endif