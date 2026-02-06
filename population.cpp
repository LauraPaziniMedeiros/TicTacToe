#include "Play.h"
#include "Bot.h"
#include <algorithm>

/* EVOLUTION CONFIGURATIONS */
#define NUM_INDIV 6 // Should be an even number
float MIN_MUT = 0.05, MAX_MUT = 0.3;
float MUTATION_STEP = (MAX_MUT - MIN_MUT)*2;
#define ROUNDS 1 // How many rounds will be played
#define CROSSOVER_ROUNDS 3 // How many rounds are played before a crossover happens

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
    vector<INDIVIDUAL> pop;
    /* The best individual in each population is always stored 
    and remains on the next population created */
    INDIVIDUAL BEST;
    // How many rounds the BEST bot has stayed the same
    int stagnation;
    // Current mutation rate
    float MUTATION_RATE;

    /**
     * @brief Updates the mutation rate based on the BEST bot's stagnation rate.
     */
    void update_mutation_rate() {
        float factor = min(1.0, stagnation / 10.0); 
        MUTATION_RATE = MIN_MUT + (MAX_MUT - MIN_MUT) * factor;
    }

    /**
     * @brief Mutates a chromossome with the current mutation rate.
     * @param genome The chromossome to be updated.
     * @return The mutated chromossome.
     */
    vector<long long> mutate(vector<long long> chromossome)
    {
        vector<long long> mutated;
        // Instead of using the old rand() from C, I am using the random library from C++. rd is the random device that we use to generate the pseudo-random numbers, and rng is the generator, using Mersenne-Twister algorithm.
        random_device rd;
        mt19937 rng(rd());

        // The chance of mutating is defined by a random applied into an uniform distribution, while the noise is defined by a random choose in a normal distribution
        uniform_real_distribution<double> chanceDist(0.0, 1.0);
        normal_distribution<double> noiseDist(0.0, MUTATION_STEP);

        // This part of the code is the same, I just changed the calculations of the noise and the ceiling for the random number to use standard C++ functions.
        for (auto &gene : chromossome)
        {
            if (chanceDist(rng) <= MUTATION_RATE)
            {
                int noise = noiseDist(rng);
                int m = gene + static_cast<int>(std::round(noise));
                m = std::max(1, m);
                mutated.push_back(m);
            }
            else
                mutated.push_back(gene);
        }
        return mutated;
    }

    /**
     * @brief Creates a new population by crossing over the best
     * individual's chromossomes with every other bot in the population.
     */
    void crossover(void) {
        // Finds the new BEST individual
        int best_idx = -1;
        for(int i = 0; i < NUM_INDIV; i++) {
            if(pop[i].wins > BEST.wins) {
                BEST = pop[i];
                stagnation = 0;
                best_idx = i;
            }
            else if(pop[i].draws > BEST.draws) {
                BEST = pop[i];
                stagnation = 0;
                best_idx = i;
            }
        }
        if(best_idx == -1)
            stagnation++;
        else {
            // Saves the previous BEST win rate
            ofstream file{"result/winrate.csv", ios_base::app};
            file << BEST.wins << "," << BEST.draws << "," << BEST.losses << endl;
        }


        // the best crosses over with every other individual and creates a new population
        vector<INDIVIDUAL> new_pop;
        new_pop.push_back(BEST);
            
        for(int i = 0; i < NUM_INDIV; i++) {
            if(i == best_idx) continue;

            // The child has all the BEST bot's chromossomes
            BOT child = BEST.bot;

            for(auto& [board_state, chromossome] : pop[i].bot.genome) {
                if(child.genome.count(board_state)) { // Both parents have this chromossome
                    // Average of both parent's chromossomes
                    for(int j = 0; j < 9; j++) {
                        child.genome[board_state][j] += chromossome[j];
                        child.genome[board_state][j] /= 2;
                    }
                }
                else // Only the current individual has this chromossome
                    child.genome[board_state] = chromossome;
                // Applies mutation
                update_mutation_rate();
                child.genome[board_state] = mutate(child.genome[board_state]);
            }
            new_pop.push_back({child, 0, 0, 0});
        }
        pop = new_pop;
    }

    public:
    /**
     * @brief creates a population of individuals with 0 wins/draws/losses.
     */
    POPULATION() : pop(NUM_INDIV), BEST(), stagnation(0), MUTATION_RATE(MIN_MUT) {
        for(int i = 0; i < NUM_INDIV; i++) {
            BOT aux;
            pop[i].bot = aux;
            pop[i].wins = pop[i].draws = pop[i].losses = 0;
        }
        BEST = pop[0];
        BEST.wins = BEST.draws = BEST.losses = 0;
    }


    /**
     * @brief Runs multiple games between bots and updates the population's genomes.
     * @param print if TRUE the board is printed at each move the bots make.
     * @param save_load if TRUE, the last results (genomes) will be loaded before the training begins
     * and saves the results after all the rounds are over.
     */
    void train_population(bool print = false, bool save_load = false) {
        if(save_load) {
            BEST.bot.load_genome("results/BEST.txt");
            for(int i = 0; i < NUM_INDIV; i += 2) {
                BOT p1('X'), p2('O');
                string file_name = "results/" + to_string(i) + ".txt";
                p1.load_genome(file_name);
                file_name = "results/" + to_string(i + 1) + ".txt";
                p2.load_genome(file_name);
                pop[i] = {p1, 0, 0, 0};
                pop[i+1] = {p2, 0, 0, 0};
            }
        }

        // Setup Random Number Generator
        auto rng = default_random_engine(time(NULL));
        for(int j = 0; j < ROUNDS; j++) {
            // This ensures random matchmaking every generation
            shuffle(pop.begin(), pop.end(), rng);

            // Simulates rounds and generates new populations
            for(int i = 0; i < NUM_INDIV; i += 2) {
                pop[i].bot.symbol = 'X';
                pop[i+1].bot.symbol = 'O';
                TicTacToeBOT game(pop[i].bot, pop[i+1].bot);
                int result = game.botVSbot(print);
                if (result == WIN) {
                    pop[i].wins++;
                    pop[i + 1].losses++;
                } else if (result == LOSS) {
                    pop[i + 1].wins++;
                    pop[i].losses++;
                } else if (result == DRAW) {
                    pop[i].draws++;
                    pop[i + 1].draws++;
                }
            }
            
            // Creates a new generation every defined number of rounds
            if(j % CROSSOVER_ROUNDS == 0) {
                crossover();
            }
                
        }
        if(save_load) {
            BEST.bot.save_genome("results/BEST.txt");
            ofstream file{"results/winrate.csv", ios::app};
            file << BEST.wins << "," << BEST.draws << "," << BEST.losses << endl;
            
            for(int i = 0; i < NUM_INDIV; i += 2) {
                string file_name = "results/" + to_string(i) + ".txt";
                pop[i].bot.save_genome(file_name);
                file_name = "results/" + to_string(i + 1) + ".txt";
                pop[i+1].bot.save_genome(file_name);
            }
        }
    }

    /**
     * @brief Runs multiple games between bots and an optimal algorithm
     * and updates the population's genomes.
     * @param print if TRUE the board is printed at each move the bots make.
     * @param save_load if TRUE, the last results (genomes) will be loaded before the training begins
     * and saves the results after all the rounds are over.
     */
    void train_population_minimax(bool print = false, bool save_load = false) {
        // (Lógica de Carregamento/Inicialização MANTIDA)
        if (save_load) {
            for (int i = 0; i < NUM_INDIV; ++i) {
                string file_name = "results/" + to_string(i) + ".txt";
                pop[i].bot.load_genome(file_name); 
                pop[i].bot.symbol = 'X'; // Definimos o símbolo base
            }
        } else {
            for (int i = 0; i < NUM_INDIV; ++i) {
                pop[i].bot.symbol = 'X'; // Definimos o símbolo base
            }
        }

        // 1. Instancia o Minimax Player fixo
        Optimal_algorithm fixed_minimax('O'); // O Minimax precisa de um símbolo para inicializar
        
        for (int j = 0; j < ROUNDS; j++) {
            
            // Simulates rounds
            for (int i = 0; i < NUM_INDIV; ++i) { // Iterar sobre todos os BOTs evolutivos
                
                // 2. Cria o controlador, passando o BOT por REFERÊNCIA
                TicTacToeMiniMax game; 

                // --- Jogo 1: BOT é 'X' (Primeiro a jogar) ---
                // 'true' significa que o BOT é 'X'
                int result_x = game.run_game(true, print); 
                
                // Atualiza a pontuação (run_gameresult_x é do ponto de vista do BOT)
                if (result_x == WIN) {
                    pop[i].wins++;
                } else if (result_x == LOSS) {
                    pop[i].losses++;
                } else if (result_x == DRAW) {
                    pop[i].draws++;
                }

                // --- Jogo 2: BOT é 'O' (Segundo a jogar) ---
                // 'false' significa que o BOT é 'O'
                int result_o = game.run_game(false, print); 
                
                // Atualiza a pontuação (result_o é do ponto de vista do BOT)
                if (result_o == WIN) {
                    pop[i].wins++;
                } else if (result_o == LOSS) {
                    pop[i].losses++;
                } else if (result_o == DRAW) {
                    pop[i].draws++;
                }
            }
            
            // 3. Impressão e Crossover
            for (int i = 0; i < NUM_INDIV; ++i)
            {
                cout << "WIN/DRAW RATE BOT " << i << " (Total): WINS: " << pop[i].wins
                    << " DRAWS: " << pop[i].draws
                    << " LOSSES: " << pop[i].losses << endl;
            }

            if (j % CROSSOVER_ROUNDS == 0 && j != 0)
                crossover();
        }
        
        // 4. Salvamento
        if (save_load) {
            BEST.bot.save_genome("results/BEST.txt");
            ofstream file{"result/winrate.csv", ios_base::app};
            file << BEST.wins << "," << BEST.draws << "," << BEST.losses << endl;
            
            for (int i = 0; i < NUM_INDIV; ++i) {
                string file_name = "results/" + to_string(i) + ".txt";
                pop[i].bot.save_genome(file_name);
            }
        }
    }

    /**
     * @brief Runs a game between the best bot and a human player, updating the population's genomes.
     * @param print if TRUE the board is printed at each move the bots make.
     * @param save_load if TRUE, the last results (genomes) will be loaded before the training begins
     * and saves the results after all the rounds are over.
     */
    void train_player(bool print = false, bool save_load = false) {
        
        // (Lógica de Carregamento/Inicialização MANTIDA)
        if (save_load) {
            BEST.bot.load_genome("results/BEST.txt");
        }

        TicTacToePlayer game(BEST.bot);
        game.run_game(true, true);
        
        // 4. Salvamento
        if (save_load) {
            BEST.bot.save_genome("results/BEST.txt");
            ofstream file{"result/winrate.csv", ios_base::app};
            file << BEST.wins << "," << BEST.draws << "," << BEST.losses << endl;
        }
    }
};

int main(void) {
    POPULATION p;

    int opc;
    
    cout << "------------ MENU -------------\n";
    cout << "Choose 1 to train the population against an optimal algorithm\n";
    cout << "Choose 2 to play against the bot\n";
    cout << "Choose 3 to have the bots compete against each other\n";
    cin >> opc;

    switch (opc)
    {
    case 1:
        p.train_population_minimax(true,true);
        break;
    
    case 2:
        p.train_player(true, true);
        break;

    case 3:
        p.train_population(true, true);
    
    default:
        break;
    }
    
    return 0;
}