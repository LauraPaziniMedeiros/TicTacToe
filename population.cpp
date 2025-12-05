#include "population.h"
#include "Play.h"
#include <algorithm>
#include <random>
#include <iostream>

using namespace std;

#define INDIVIDUALS 6
float MIN_MUT = 0.05, MAX_MUT = 0.3;
float MUTATION_STEP = (MAX_MUT - MIN_MUT)*2;
#define ROUNDS 100
#define CROSSOVER_ROUNDS 5

POPULATION::POPULATION() : pop(INDIVIDUALS), BEST(), stagnation(0), MUTATION_RATE(MIN_MUT) {
    for(int i = 0; i < INDIVIDUALS; i += 2) {
        BOT aux('X');
        pop[i] = {aux, 0};
    }
    BEST = {pop[0]};
}

void POPULATION::update_mutation_rate() {
    float factor = min(1.0, stagnation / 10.0); 
    MUTATION_RATE = MIN_MUT + (MAX_MUT - MIN_MUT) * factor;
}

vector<long long> POPULATION::mutate(vector<long long> genome)
{
    vector<long long> mutated;
    random_device rd;
    mt19937 rng(rd());
    uniform_real_distribution<double> chanceDist(0.0, 1.0);
    normal_distribution<double> noiseDist(0.0, MUTATION_STEP);

    for (auto &g : genome) {
        if (chanceDist(rng) <= MUTATION_RATE) {
            int noise = noiseDist(rng);
            int m = g + static_cast<int>(std::round(noise));
            m = std::max(1, m);
            mutated.push_back(m);
        }
        else
            mutated.push_back(g);
    }
    return mutated;
}

void POPULATION::crossover(void) {
    sort(pop.begin(), pop.end(), 
        [](const pair<BOT, int>& a, const pair<BOT, int>& b) {return a.second > b.second;});

    vector<pair<BOT, int>> new_pop;
    if(BEST.second < pop[0].second) {
        BEST = pop[0];
        stagnation = 0;
    }
    else
        stagnation++;

    new_pop.push_back(BEST);
        
    for(int i = 1; i < INDIVIDUALS; i++) {
        BOT child;
        child.symbol = 'X';
        child.genomes = BEST.first.genomes;

        for(auto& [board_state, genome] : pop[i].first.genomes) {
            if(child.genomes.count(board_state)) { 
                for(int j = 0; j < 9; j++) {
                    child.genomes[board_state][j] += genome[j];
                    child.genomes[board_state][j] /= 2;
                }
            }
            else 
                child.genomes[board_state] = genome;
            update_mutation_rate();
            child.genomes[board_state] = mutate(child.genomes[board_state]);
        }
        new_pop.push_back({child, (BEST.second + pop[i].second) / 2});
    }
    pop = new_pop;
}

void POPULATION::train_population(bool print, bool save_load) {
    if(save_load) {
        for(int i = 0; i < INDIVIDUALS; i += 2) {
            BOT p1('X'), p2('O');
            string file_name = "X";
            file_name.push_back(i + '0');
            file_name.append(".txt");
            p1.load_genomes(file_name);
            file_name[0] = 'O';
            p1.load_genomes(file_name);
            pop[i] = {p1, 0};
            pop[i+1] = {p2, 0};
        }
    }

    auto rng = default_random_engine(time(NULL));
    vector<pair<int, pair<int, int>>> winrate_table(INDIVIDUALS, {0, {0, 0}});

    for(int j = 0; j < ROUNDS; j++) {
        shuffle(pop.begin(), pop.end(), rng);

        for(int i = 0; i < INDIVIDUALS; i += 2) {
            pop[i].first.symbol = 'X';
            pop[i+1].first.symbol = 'O';
            TicTacToeBOT game(pop[i].first, pop[i+1].first);
            int result = game.botVSbot(print);
            if (result == WIN) {
                pop[i].second += 1;
                pop[i+1].second -= 1;
                winrate_table[i].first++;
                winrate_table[i+1].second.second++;
            } else if (result == LOSS) {
                pop[i].second -= 1;
                pop[i + 1].second += 1;
                winrate_table[i + 1].first++;
                winrate_table[i].second.second++;
            } else if (result == DRAW) {
                winrate_table[i].second.first++;
                winrate_table[i+1].second.first++;
            }
        }
        for (int i = 0; i < INDIVIDUALS; ++i) {
            cout << "WIN/DRAW RATE BOT " << i << ": WINS: " << winrate_table[i].first << " DRAWS: " << winrate_table[i].second.first << " LOSSES: " << winrate_table[i].second.second << endl;
        }
        if(j % CROSSOVER_ROUNDS == 0) 
            crossover();
    }
    if(save_load) {
        for(int i = 0; i < INDIVIDUALS; i += 2) {
            string file_name = "";
            file_name += pop[i].first.symbol;
            file_name += i + '0' ;
            file_name.append(".txt");
            pop[i].first.save_genomes(file_name);
            file_name[0] = pop[i+1].first.symbol;
            pop[i+1].first.save_genomes(file_name);
        }
    }        
}

void POPULATION::train_population_minimax(bool print, bool save_load) {
    if (save_load) {
        for (int i = 0; i < INDIVIDUALS; ++i) {
            string file_name = "X" + to_string(i) + ".txt";
            pop[i].first.load_genomes(file_name); 
            pop[i].first.symbol = 'X'; 
        }
    } else {
        for (int i = 0; i < INDIVIDUALS; ++i) {
            pop[i].first.symbol = 'X'; 
        }
    }

    Optimal_algorithm fixed_minimax('O'); 
    vector<pair<int, pair<int, int>>> winrate_table(INDIVIDUALS, {0, {0, 0}}); 

    for (int j = 0; j < ROUNDS; j++) {
        for (int i = 0; i < INDIVIDUALS; ++i) { 
            TicTacToeMiniMax game(pop[i].first, fixed_minimax); 
            int result_x = game.run_game(true, print); 
            
            if (result_x == WIN) {
                pop[i].second += 1;
                winrate_table[i].first++;
            } else if (result_x == LOSS) {
                pop[i].second -= 1;
                winrate_table[i].second.second++;
            } else if (result_x == DRAW) {
                winrate_table[i].second.first++;
            }

            int result_o = game.run_game(false, print); 
            
            if (result_o == WIN) {
                pop[i].second += 1;
                winrate_table[i].first++;
            } else if (result_o == LOSS) {
                pop[i].second -= 1;
                winrate_table[i].second.second++;
            } else if (result_o == DRAW) {
                winrate_table[i].second.first++;
            }
        }
        
        for (int i = 0; i < INDIVIDUALS; ++i) {
            cout << "WIN/DRAW RATE BOT " << i << " (Total): WINS: " << winrate_table[i].first 
                 << " DRAWS: " << winrate_table[i].second.first 
                 << " LOSSES: " << winrate_table[i].second.second << endl;
        }

        if (j % CROSSOVER_ROUNDS == 0 && j != 0)
            crossover();
            
        for(int i = 0; i < INDIVIDUALS; i++){
            winrate_table[i] = {0, {0, 0}};
        }
    }
    
    if (save_load) {
        for (int i = 0; i < INDIVIDUALS; ++i) {
            string file_name = "X" + to_string(i) + ".txt";
            pop[i].first.save_genomes(file_name);
        }
    }
}