#ifndef POPULATION_H
#define POPULATION_H

#include "Bot.h"
#include <vector>

class POPULATION {
    private:
    vector<pair<BOT, int>> pop;
    pair<BOT, int> BEST;
    int stagnation;
    float MUTATION_RATE;

    void update_mutation_rate();
    vector<long long> mutate(vector<long long> genome);
    void crossover(void);

    public:
    POPULATION();
    void train_population(bool print = false, bool save_load = false);
    void train_population_minimax(bool print = false, bool save_load = false);
};

#endif