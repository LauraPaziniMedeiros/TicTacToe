#include "genetic_algorithm.cpp"
#include <algorithm>

// Config
#define INDIVIDUALS 6
float MUTATION_RATE = 0.1;
#define ROUNDS 10
#define CROSSOVER_ROUNDS 10

class POPULATION {
    private:
    // Stores the population and each individual's win rate
    vector<pair<BOT, int>> pop;
    // The best individual is always stored and unchanged
    pair<BOT, int> BEST;

    public:
    /**
     * @brief creates a population of bots (alternating symbols) and with 0 wins.
     */
    POPULATION() : pop(INDIVIDUALS), BEST() {
        for(int i = 0; i < INDIVIDUALS; i += 2) {
            BOT aux('X');
            pop[i] = {aux, 0};
        }
        for(int i = 1; i < INDIVIDUALS; i += 2) {
            BOT aux('O');
            pop[i] = {aux, 0};
        }
        
        BEST = {pop[0]};
    }

    vector<int> mutation(vector<int> genome) {
        vector<int> mutated;
        for(auto& g : genome) {
            float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            if(r <= MUTATION_RATE) {
                int m = (1 + MUTATION_RATE) * g;
                int noise = (rand() % m) - m/2;
                m = g + noise;
                if(m < 1) m = 1;
                mutated.push_back(m);
            }
            else
                mutated.push_back(g);        
        }
        return mutated;
    }

    void crossover(void) {
        // Sorting based on win rate
        sort(pop.begin(), pop.end(), 
            [](const pair<BOT, int>& a, const pair<BOT, int>& b) {return a.second > b.second;});

        // the best crosses over with every other individual and creates a new population
        vector<pair<BOT, int>> new_pop;
        if(BEST.second < pop[0].second) 
            BEST = pop[0];
        
        for(int i = 0; i < INDIVIDUALS; i++) {
            BOT child;
            if(i % 2) child.symbol = 'X'; else child.symbol = 'O';
            // The child now knows everything the best individual does
            child.genomes = BEST.first.genomes;

            for(auto& [board_state, genome] : pop[i].first.genomes) {
                if(child.genomes.count(board_state)) { // Both parents have this genome
                    // 50% chance of inheriting each parent's genome
                    if (rand() % 2 == 0) // Keeps this individual's genome
                        child.genomes[board_state] = genome;
                    // Else: Keeps best individual's genome
                }
                else { // Only the current individual has this genome
                    child.genomes[board_state] = genome;
                }                                
            }
            new_pop.push_back({child, (BEST.second + pop[i].second) / 2});
        }
        pop = new_pop;
    }

    void train_population(bool print = false, bool save_load = false) {
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

        // Setup Random Number Generator
        auto rng = default_random_engine(time(NULL));
        for(int j = 0; j < ROUNDS; j++) {
            // This ensures random matchmaking every generation
            shuffle(pop.begin(), pop.end(), rng);

            // Simulates rounds and generates new populations
            for(int i = 0; i < INDIVIDUALS; i += 2) {
                pop[i].first.symbol = 'X';
                pop[i+1].first.symbol = 'O';
                TicTacToeBOT game(pop[i].first, pop[i+1].first);
                int result = game.botVSbot(print);
                if(result == DRAW) {
                    pop[i] = {game.players[0], pop[i].second};
                    pop[i+1] = {game.players[1], pop[i+1].second};
                }
                else if(result == WIN) { // X won
                    pop[i] = {game.players[0], pop[i].second + 1};
                    pop[i+1] = {game.players[1], pop[i+1].second - 1};
                }
                else { // O won
                    pop[i] = {game.players[0], pop[i].second - 1};
                    pop[i+1] = {game.players[1], pop[i+1].second + 1};
                }
            }
            for(int i = 0; i < INDIVIDUALS; i += 2) {
                cout << "WIN RATE BOT " << i << ": " << pop[i].second << endl;
                cout << "WIN RATE BOT " << i+1 << ": " << pop[i+1].second << endl;
            }
            if(j % CROSSOVER_ROUNDS == 0) // Creates a new generation every 5 rounds
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
};

int main(void) {
    POPULATION p;
    p.train_population(true, true);
    return 0;
}