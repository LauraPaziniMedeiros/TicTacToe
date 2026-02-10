#ifndef POP_H
    #define POP_H
    #include "TicTacToe.h"
    #include <algorithm>
    #include <chrono>
    #include <tuple>

    /* EVOLUTION CONFIGURATIONS */
    #define NUM_INDIV 8 // Should be an even number
    const float MIN_MUT = 0.05, MAX_MUT = 0.3;
    const float MUTATION_STEP = (MAX_MUT - MIN_MUT)*2;
    #define ROUNDS 4 // How many rounds will be played
    #define CROSSOVER_ROUNDS 2 // How many rounds are played before a crossover happens

    /**
     * @struct INDIVIDUAL
     * @brief Defines an individual in the population,
     * represented by a bot and it's win rate.
     */
    typedef struct {
        BOT bot;
        int wins;
        int draws;
        int losses;
    } INDIVIDUAL;

    class POPULATION {
        private:
        // Stores the population and each individual's win rate
        vector<INDIVIDUAL> popX, popO;
        /* The best individual in each population is always stored 
        and remains on the next population created */
        INDIVIDUAL BESTX, BESTO;
        // How many rounds the BEST bot has stayed the same
        int stagnationX, stagnationO;
        // Current mutation rate
        float mutation_rateX, mutation_rateO;

        void update_mutation_rate(const bool& is_X);
        vector<unsigned long long> mutate(const vector<unsigned long long>& chromossome, const bool& is_X);
        void crossover(const bool& save_load);
        void load_best(void);
        void save_best(void);

        public:
        POPULATION(void);
        void train_botvsbot(bool print = false, bool save_load = true);
        void train_botvsminimax(bool print = false, bool save_load = true);
        void train_botvsplayer(bool print = false, bool save_load = true, bool is_X = true);
    };
#endif