#include "header_files/Population.h"

/**
 * @brief Updates the mutation rate based on the BEST bot's stagnation rate.
 * @param is_X Whether to use X's stagnation rate or O's.
 */
void POPULATION::update_mutation_rate(const bool& is_X) {
    if(is_X) {
        float factor = min(1.0, stagnationX / 10.0); 
        mutation_rateX = MIN_MUT + (MAX_MUT - MIN_MUT) * factor;
    } else {
        float factor = min(1.0, stagnationO / 10.0); 
        mutation_rateO = MIN_MUT + (MAX_MUT - MIN_MUT) * factor;
    }
    
}

/**
 * @brief Mutates a chromossome with the current mutation rate for each symbol.
 * @param chromossome The chromossome to be updated.
 * @param is_X Whether to use X's stagnation rate or O's.
 * @return The mutated chromossome.
 */
vector<unsigned long long> POPULATION::mutate(const vector<unsigned long long>& chromossome, const bool& is_X)
{
    vector<unsigned long long> mutated;
    mutated.reserve(chromossome.size()); // Pre-allocate memory to speed up push_back
    unsigned long long mutation_rate = is_X ? mutation_rateX : mutation_rateO;

    for (auto &gene : chromossome)
    {
        if (Random64::probability() <= mutation_rate)
        {
            double noise = Random64::deviation(0.0, MUTATION_STEP);
            long long noiseInt = std::llround(noise);
            /* If noise is negative AND its magnitude is greater than the gene,
            the result would be negative. We clamp it to 1. */
            if (noiseInt < 0 && gene < (unsigned long long)(-noiseInt)) {
                mutated.push_back(1);
            } 
            else {
                unsigned long long m = gene + noiseInt;
                // Final clamp to ensure we never drop below 1
                mutated.push_back(m < 1 ? 1 : m);
            }
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
void POPULATION::crossover(void) {
    // Finds the new BEST individual for X's population
    int bestX_idx = -1;
    for(int i = 0; i < NUM_INDIV/2; i++) {
        if(popX[i].wins > BESTX.wins) {
            BESTX = popX[i];
            stagnationX = 0;
            bestX_idx = i;
        }
        else if(popX[i].draws > BESTX.draws) {
            BESTX = popX[i];
            stagnationX = 0;
            bestX_idx = i;
        }
    }
    if(bestX_idx == -1)
        stagnationX++;
    else {
        // Saves the previous BEST win rate
        ofstream file{"result/winrateX.csv", ios_base::app};
        file << BESTX.wins << "," << BESTX.draws << "," << BESTX.losses << endl;
    }

    // Finds the new BEST individual for O's population
    int bestO_idx = -1;
    for(int i = 0; i < NUM_INDIV/2; i++) {
        if(popO[i].wins > BESTO.wins) {
            BESTO = popO[i];
            stagnationO = 0;
            bestO_idx = i;
        }
        else if(popO[i].draws > BESTO.draws) {
            BESTO = popO[i];
            stagnationO = 0;
            bestO_idx = i;
        }
    }
    if(bestO_idx == -1)
        stagnationO++;
    else {
        // Saves the previous BEST win rate
        ofstream file{"result/winrateO.csv", ios_base::app};
        file << BESTO.wins << "," << BESTO.draws << "," << BESTO.losses << endl;
    }

    // the best crosses over with every other individual and creates a new population
    vector<INDIVIDUAL> new_popX, new_popO;
    new_popX.push_back(BESTX);
    new_popO.push_back(BESTO);

    // X's population    
    for(int i = 0; i < NUM_INDIV/2; i++) {
        if(i == bestX_idx) continue;

        // The child has all the BEST bot's chromossomes
        BOT child = BESTX.bot;

        for(auto& [board_state, chromossome] : popX[i].bot.genome) {
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
            update_mutation_rate(true);
            child.genome[board_state] = mutate(child.genome[board_state], true);
        }
        new_popX.push_back({child, 0, 0, 0});
    }
    popX = new_popX;

    // O's population    
    for(int i = 0; i < NUM_INDIV/2; i++) {
        if(i == bestO_idx) continue;

        // The child has all the BEST bot's chromossomes
        BOT child = BESTO.bot;

        for(auto& [board_state, chromossome] : popO[i].bot.genome) {
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
            update_mutation_rate(false);
            child.genome[board_state] = mutate(child.genome[board_state], false);
        }
        new_popO.push_back({child, 0, 0, 0});
    }
    popO = new_popO;
}

/**
 * @brief creates a population of individuals with 0 wins/draws/losses.
 */
POPULATION::POPULATION(void) : popX(NUM_INDIV/2), popO(NUM_INDIV/2), BESTX(), BESTO(),
stagnationX(0), stagnationO(0), mutation_rateX(MIN_MUT), mutation_rateO(MIN_MUT) {
    for(int i = 0; i < NUM_INDIV/2; i++) {
        BOT aux('X');
        popX[i].bot = aux;
        popX[i].wins = popX[i].draws = popX[i].losses = 0;
    }
    BESTX = popX[0];
    BESTX.wins = BESTX.draws = BESTX.losses = 0;

    for(int i = 0; i < NUM_INDIV/2; i++) {
        BOT aux('O');
        popO[i].bot = aux;
        popO[i].wins = popO[i].draws = popO[i].losses = 0;
    }
    BESTO = popO[0];
    BESTO.wins = BESTO.draws = BESTO.losses = 0;
}

/**
 * @brief Runs multiple games between bots and updates the population's genomes.
 * @param print if TRUE the board is printed at each move the bots make.
 * @param save_load if TRUE, the last results (genomes) will be loaded before the training begins
 * and saves the results after all the rounds are over.
 */
void POPULATION::train_botvsbot(bool print, bool save_load) {
    if(save_load) {
        BESTX.bot.load_genome("results/BESTX.txt");
        BESTO.bot.load_genome("results/BESTO.txt");

        string filename;
        for(int i = 0; i < NUM_INDIV/2; i++) {
            filename = "results/X" + to_string(i) + ".txt";
            popX[i].bot.load_genome(filename);
            filename = "results/O" + to_string(i) + ".txt";
            popO[i].bot.load_genome(filename);                
        }
    }

    // Setup Random Number Generator
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    for(int j = 0; j < ROUNDS; j++) {
        // This ensures random matchmaking every round
        shuffle(popX.begin(), popX.end(), rng);
        shuffle(popO.begin(), popO.end(), rng);

        // Simulates rounds and generates new populations
        for(int i = 0; i < NUM_INDIV/2; i++) {
            BOTvsBOT game(popX[i].bot, popO[i].bot);
            int result = game.run_game(print);
            if (result == WIN) {
                popX[i].wins++;
                popO[i].losses++;
            } else if (result == LOSS) {
                popX[i].losses++;
                popO[i].wins++;
            } else if (result == DRAW) {
                popX[i].draws++;
                popO[i].draws++;
            }
        }
        
        // Creates a new generation every defined number of rounds
        if(j % CROSSOVER_ROUNDS == 0) {
            crossover();
        }
            
    }

    if(save_load) {
        BESTX.bot.save_genome("results/BESTX.txt");
        BESTO.bot.save_genome("results/BESTO.txt");

        string filename;
        for(int i = 0; i < NUM_INDIV/2; i++) {
            filename = "results/X" + to_string(i) + ".txt";
            popX[i].bot.save_genome(filename);
            filename = "results/O" + to_string(i) + ".txt";
            popO[i].bot.save_genome(filename);                
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
void POPULATION::train_botvsminimax(bool print, bool save_load) {
    // (Lógica de Carregamento/Inicialização MANTIDA)
    if (save_load) {
        BESTX.bot.load_genome("results/BESTX.txt");
        BESTO.bot.load_genome("results/BESTO.txt");

        string filename;
        for(int i = 0; i < NUM_INDIV/2; i++) {
            filename = "results/X" + to_string(i) + ".txt";
            popX[i].bot.load_genome(filename);
            filename = "results/O" + to_string(i) + ".txt";
            popO[i].bot.load_genome(filename);                
        }
    }

    for (int j = 0; j < ROUNDS; j++) {
        for (int i = 0; i < NUM_INDIV/2; ++i) {
            // X plays first
            BOTvsMINIMAX gameX(popX[i].bot);
            int result_x = gameX.run_game(print);
            
            if (result_x == WIN) {
                popX[i].wins++;
            } else if (result_x == LOSS) {
                popX[i].losses++;
            } else if (result_x == DRAW) {
                popX[i].draws++;
            }

            // O plays second
            BOTvsMINIMAX gameO(popO[i].bot);
            int result_o = gameO.run_game(print); 
            
            if (result_o == WIN) {
                popO[i].wins++;
            } else if (result_o == LOSS) {
                popO[i].losses++;
            } else if (result_o == DRAW) {
                popO[i].draws++;
            }
        }
        
        for (int i = 0; i < NUM_INDIV/2; ++i) {
            cout << "WIN/DRAW RATE BOT X" << i << " (Total): WINS: " << popX[i].wins
                << " DRAWS: " << popX[i].draws
                << " LOSSES: " << popX[i].losses << endl;
            cout << "WIN/DRAW RATE BOT O" << i << " (Total): WINS: " << popO[i].wins
                << " DRAWS: " << popO[i].draws
                << " LOSSES: " << popO[i].losses << endl;
        }

        if (j % CROSSOVER_ROUNDS == 0)
            crossover();
    }
    
    // 4. Salvamento
    if (save_load) {
        BESTX.bot.save_genome("results/BESTX.txt");
        BESTO.bot.save_genome("results/BESTO.txt");

        string filename;
        for(int i = 0; i < NUM_INDIV/2; i++) {
            filename = "results/X" + to_string(i) + ".txt";
            popX[i].bot.save_genome(filename);
            filename = "results/O" + to_string(i) + ".txt";
            popO[i].bot.save_genome(filename);                
        }
    }
}

/**
 * @brief Runs a game between the best bot and a human player, updating the population's genomes.
 * @param print if TRUE the board is printed at each move the bots make.
 * @param save_load if TRUE, the last results (genomes) will be loaded before the training begins
 * @param is_X Whether the player is represented by 'X' or 'O'.
 * and saves the results after all the rounds are over.
 */
void POPULATION::train_botvsplayer(bool print, bool save_load, bool is_X) {
    if(is_X) {
        if(save_load) BESTO.bot.load_genome("results/BESTO.txt");

        BOTvsPLAYER game(BESTO.bot);
        int result = game.run_game(true);
        if(result == WIN) BESTO.wins++;
        else if(result == LOSS) BESTO.losses++;
        else BESTO.draws++;

        if(save_load) BESTO.bot.save_genome("results/BESTO.txt");
    } else {
        if(save_load) BESTX.bot.load_genome("results/BESTX.txt");

        BOTvsPLAYER game(BESTX.bot);
        int result = game.run_game(true);
        if(result == WIN) BESTX.wins++;
        else if(result == LOSS) BESTX.losses++;
        else BESTX.draws++;

        if(save_load) BESTX.bot.save_genome("results/BESTX.txt");
    }
}