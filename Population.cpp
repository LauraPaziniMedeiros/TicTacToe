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
void POPULATION::crossover(const bool& save_load) {
    // Sorts the population according to their win rate
    sort(popX.begin(), popX.end(), [](const INDIVIDUAL& a, const INDIVIDUAL& b){
        if(a.wins != b.wins) return a.wins > b.wins;
        else if(a.draws != b.draws) return a.draws > b.draws;
        return a.bot.genome.size() > b.bot.genome.size();
    });
    sort(popO.begin(), popO.end(), [](const INDIVIDUAL& a, const INDIVIDUAL& b){
        if(a.wins != b.wins) return a.wins > b.wins;
        else if(a.draws != b.draws) return a.draws > b.draws;
        return a.bot.genome.size() > b.bot.genome.size();
    });

    // Updates BEST
    auto candidate_stats = make_tuple(popX[0].wins, popX[0].draws, popX[0].bot.genome.size());
    auto best_stats = make_tuple(BESTX.wins, BESTX.draws, BESTX.bot.genome.size());
    if(candidate_stats > best_stats) {
        BESTX = popX[0];
        stagnationX = 0;
    } else stagnationX++;
    candidate_stats = make_tuple(popO[0].wins, popO[0].draws, popO[0].bot.genome.size());
    best_stats = make_tuple(BESTO.wins, BESTO.draws, BESTO.bot.genome.size());
    if(candidate_stats > best_stats) {
        BESTO = popO[0];
        stagnationO = 0;
    } else stagnationO++;

    /* Saves the best win rates for every new population
    along with the BEST individuals stagnation rates*/
    if(save_load) {
        ofstream file{"results/ALLwinrateX.bin", ios_base::app};
        if(!file.is_open()) {
            cout << "Could not open file \"results/ALLwinrateX.bin\" for writing\n";
            return;
        }
        file << stagnationX << ",";
        file << BESTX.wins << "," << BESTX.draws << "," << BESTX.losses << endl;
        file.close();

        file.open("results/ALLwinrateO.bin", ios_base::app);
        if(!file.is_open()) {
            cout << "Could not open file \"results/ALLwinrateO.bin\" for writing\n";
            return;
        }
        file << stagnationO << ",";
        file << BESTO.wins << "," << BESTO.draws << "," << BESTO.losses << endl;
        file.close();
    }

    // Adjusts population for crossover
    if(stagnationX == 0) popX.erase(popX.begin()); // New BEST
    else popX.pop_back(); // Discards worst individual
    if(stagnationO == 0) popO.erase(popO.begin()); // New BEST
    else popO.pop_back(); // Discards worst individual

    // the best reproduces with every other individual and creates a new population
    vector<INDIVIDUAL> new_popX, new_popO;
    new_popO.reserve(NUM_INDIV/2); new_popX.reserve(NUM_INDIV/2);
    new_popX.push_back({BESTX.bot, 0, 0, 0});
    new_popO.push_back({BESTO.bot, 0, 0, 0});

    for(int i = 0; i < NUM_INDIV/2 - 1; i++) {
        // X's population   
        // The child has all the BEST bot's chromossomes
        BOT child = BESTX.bot;
        for(auto& [board_state, chromossome] : popX[i].bot.genome) {
            if(child.genome.count(board_state)) { // Both parents have this chromossome
                // Average of both parent's chromossomes
                for(int j = 0; j < 9; j++) {
                    child.genome[board_state][j] += chromossome[j];
                    child.genome[board_state][j] /= 2;
                }
            } else // Only the current individual has this chromossome
                child.genome[board_state] = chromossome;
            // Applies mutation to each gene
            update_mutation_rate(true);
            for(auto& [board_state, chromossome] : child.genome) {
                chromossome = mutate(chromossome, true);
            }
        }
        new_popX.push_back({child, 0, 0, 0});

        // O's population
        // The child has all the BEST bot's chromossomes
        child = BESTO.bot;
        for(auto& [board_state, chromossome] : popO[i].bot.genome) {
            if(child.genome.count(board_state)) { // Both parents have this chromossome
                // Average of both parent's chromossomes
                for(int j = 0; j < 9; j++) {
                    child.genome[board_state][j] += chromossome[j];
                    child.genome[board_state][j] /= 2;
                }
            } else // Only the current individual has this chromossome
                child.genome[board_state] = chromossome;
            // Applies mutation to each gene
            update_mutation_rate(false);
            for(auto& [board_state, chromossome] : child.genome) {
                chromossome = mutate(chromossome, false);
            }
        }
        new_popO.push_back({child, 0, 0, 0});
    }
    popX.clear(); popX = new_popX;
    popO.clear(); popO = new_popO;
}

/**
 * @brief Loads the last recorded BEST individuals and their stagnation rates.
 */
void POPULATION::load_best(void) {
    ifstream file("results/BESTwinrateX.bin");
    if(!file.is_open()) {
        cout << "Could not open file \"results/BESTwinrateX.bin\" for reading\n";
        return;
    }
    string line;
    getline(file, line);
    stringstream ssX(line);
    char separator;
    ssX >> stagnationX >> separator;
    ssX >> BESTX.wins >> separator >> BESTX.draws >> separator >> BESTX.losses;
    file.close();

    file.open("results/BESTwinrateO.bin");
    if(!file.is_open()) {
        cout << "Could not open file \"results/BESTwinrateO.bin\" for reading\n";
        return;
    }
    getline(file, line);
    stringstream ssO(line);
    ssO >> stagnationO >> separator;
    ssO >> BESTO.wins >> separator >> BESTO.draws >> separator >> BESTO.losses;

    BESTX.bot.load_genome("results/BESTX.txt");
    BESTO.bot.load_genome("results/BESTO.txt");
}

/**
 * @brief Saves the BEST individuals along with their stagnation rates.
 */
void POPULATION::save_best(void) {
    ofstream file("results/BESTwinrateX.bin");
    if(!file.is_open()) {
        cout << "Could not open file \"results/BESTwinrateX.bin\" for writing\n";
        return;
    }
    file << stagnationX << ",";
    file << BESTX.wins << "," << BESTX.draws << "," << BESTX.losses << endl;
    file.close();

    file.open("results/BESTwinrateO.bin");
    if(!file.is_open()) {
        cout << "Could not open file \"results/BESTwinrateO.bin\" for writing\n";
        return;
    }
    file << stagnationO << ",";
    file << BESTO.wins << "," << BESTO.draws << "," << BESTO.losses << endl;
    file.close();

    BESTX.bot.save_genome("results/BESTX.txt");
    BESTO.bot.save_genome("results/BESTO.txt");
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
    BESTX.wins = BESTX.draws = BESTX.losses = -1;

    for(int i = 0; i < NUM_INDIV/2; i++) {
        BOT aux('O');
        popO[i].bot = aux;
        popO[i].wins = popO[i].draws = popO[i].losses = 0;
    }
    BESTO = popO[0];
    BESTO.wins = BESTO.draws = BESTO.losses = -1;
}

/**
 * @brief Runs multiple games between bots and updates the population's genomes.
 * @param print if TRUE the board is printed at each move the bots make.
 * @param save_load if TRUE, the last results (genomes) will be loaded before the training begins
 * and saves the results after all the rounds are over.
 */
void POPULATION::train_botvsbot(bool print, bool save_load) {
    if(save_load) {
        load_best();
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
    for(int j = 1; j <= ROUNDS; j++) {
        // This ensures random matchmaking every round
        shuffle(popX.begin(), popX.end(), rng);

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
            // Prints win rates before creating a new population
            if(print) {
                for(int i = 0; i < NUM_INDIV/2; i++) {
                    cout << "WIN/DRAW RATE BOT X" << i << " (Total): WINS: " << popX[i].wins
                        << " DRAWS: " << popX[i].draws
                        << " LOSSES: " << popX[i].losses << endl;
                    cout << "WIN/DRAW RATE BOT O" << i << " (Total): WINS: " << popO[i].wins
                        << " DRAWS: " << popO[i].draws
                        << " LOSSES: " << popO[i].losses << endl;
                }
            }
            
            crossover(save_load);
        }
            
    }

    if(save_load) {
        save_best();
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
    if (save_load) {
        load_best();
        string filename;
        for(int i = 0; i < NUM_INDIV/2; i++) {
            filename = "results/X" + to_string(i) + ".txt";
            popX[i].bot.load_genome(filename);
            filename = "results/O" + to_string(i) + ".txt";
            popO[i].bot.load_genome(filename);                
        }
    }

    for (int j = 1; j <= ROUNDS; j++) {
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

        if (j % CROSSOVER_ROUNDS == 0) {
            // Prints win rates before creating a new population
            if(print) {
                for(int i = 0; i < NUM_INDIV/2; i++) {
                    cout << "WIN/DRAW RATE BOT X" << i << " (Total): WINS: " << popX[i].wins
                        << " DRAWS: " << popX[i].draws
                        << " LOSSES: " << popX[i].losses << endl;
                    cout << "WIN/DRAW RATE BOT O" << i << " (Total): WINS: " << popO[i].wins
                        << " DRAWS: " << popO[i].draws
                        << " LOSSES: " << popO[i].losses << endl;
                }
            }

            crossover(save_load);
        }
    }
    
    // 4. Salvamento
    if (save_load) {
        save_best();
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