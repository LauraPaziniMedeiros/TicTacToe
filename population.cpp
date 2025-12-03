#include "genetic_algorithm.cpp"
#include <algorithm>

// Config
#define INDIVIDUALS 6
float MIN_MUT = 0.05, MAX_MUT = 0.3;
float MUTATION_STEP = (MAX_MUT - MIN_MUT)*2;
#define ROUNDS 10
#define CROSSOVER_ROUNDS 5

class POPULATION {
    private:
    // Stores the population and each individual's win rate
    vector<pair<BOT, int>> pop;
    // The best individual is always stored and unchanged
    pair<BOT, int> BEST;
    // How many rounds the BEST bot has stayed the same
    int stagnation;
    // Current mutation rate
    float MUTATION_RATE;

    public:
    /**
     * @brief creates a population of bots (alternating symbols) and with 0 wins.
     */
    POPULATION() : pop(INDIVIDUALS), BEST(), stagnation(0), MUTATION_RATE(MIN_MUT) {
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

    void update_mutation_rate() {
        float factor = min(1.0, stagnation / 10.0); 

        MUTATION_RATE = MIN_MUT + (MAX_MUT - MIN_MUT) * factor;
    }

    vector<int> mutate(vector<int> genome)
    {
        vector<int> mutated;
        // Instead of using the old rand() from C, I am using the random library from C++. rd is the random device that we use to generate the pseudo-random numbers, and rng is the generator, using Mersenne-Twister algorithm.
        std::random_device rd;
        std::mt19937 rng(rd());

        // The chance of mutating is defined by a random applied into an uniform distribution, while the noise is defined by a random choose in a normal distribution
        std::uniform_real_distribution<double> chanceDist(0.0, 1.0);
        std::normal_distribution<double> noiseDist(0.0, MUTATION_STEP);

        // This part of the code is the same, I just changed the calculations of the noise and the ceiling for the random number to use standard C++ functions.
        for (auto &g : genome)
        {
            if (chanceDist(rng) <= MUTATION_RATE)
            {
                int noise = noiseDist(rng);
                int m = g + static_cast<int>(std::round(noise));
                m = std::min(1, m);
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
        // Updates BEST and the stagnation rate
        if(BEST.second < pop[0].second) {
            BEST = pop[0];
            stagnation = 0;
        }
        else
            stagnation++;
            
        
        for(int i = 0; i < INDIVIDUALS; i++) {
            BOT child;
            if(i % 2) child.symbol = 'X'; else child.symbol = 'O';
            // The child has all the BEST's genomes
            child.genomes = BEST.first.genomes;

            for(auto& [board_state, genome] : pop[i].first.genomes) {
                if(child.genomes.count(board_state)) { // Both parents have this genome
                    // Average of both parent's genomes
                    for(int i = 0; i < 9; i++) {
                        child.genomes[board_state][i] += genome[i];
                        child.genomes[board_state][i] /= 2;
                    }
                }
                else // Only the current individual has this genome
                    child.genomes[board_state] = genome;
                // Applies mutation
                update_mutation_rate();
                child.genomes[board_state] = mutate(child.genomes[board_state]);
            }
            // Win rate is the average between the parent's last win rate
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
            if(j % CROSSOVER_ROUNDS == 0) // Creates a new generation every defined number of rounds
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