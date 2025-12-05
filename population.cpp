#include "Play.cpp"
#include <algorithm>

// Config
#define INDIVIDUALS 2
float MIN_MUT = 0.05, MAX_MUT = 0.3;
float MUTATION_STEP = (MAX_MUT - MIN_MUT)*2;
#define ROUNDS 6
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
        for(int i = 0; i < INDIVIDUALS; i++) {
            BOT aux('X');
            pop[i] = {aux, 0};
        }
        
        BEST = {pop[0]};
        BEST.second = INT32_MIN;
    }

    void update_mutation_rate() {
        float factor = min(1.0, stagnation / 10.0); 

        MUTATION_RATE = MIN_MUT + (MAX_MUT - MIN_MUT) * factor;
    }

    vector<long long> mutate(vector<long long> genome)
    {
        vector<long long> mutated;
        // Instead of using the old rand() from C, I am using the random library from C++. rd is the random device that we use to generate the pseudo-random numbers, and rng is the generator, using Mersenne-Twister algorithm.
        random_device rd;
        mt19937 rng(rd());

        // The chance of mutating is defined by a random applied into an uniform distribution, while the noise is defined by a random choose in a normal distribution
        uniform_real_distribution<double> chanceDist(0.0, 1.0);
        normal_distribution<double> noiseDist(0.0, MUTATION_STEP);

        // This part of the code is the same, I just changed the calculations of the noise and the ceiling for the random number to use standard C++ functions.
        for (auto &g : genome)
        {
            if (chanceDist(rng) <= MUTATION_RATE)
            {
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

    void crossover(void) {
        // Sorting based on win rate
        sort(pop.begin(), pop.end(), 
            [](const pair<BOT, int>& a, const pair<BOT, int>& b) {return a.second > b.second;});

        // the best crosses over with every other individual and creates a new population
        vector<pair<BOT, int>> new_pop;
        // Updates BEST and the stagnation rate
        if(BEST.second < pop[0].second) {
            this->BEST.first = pop[0].first;
            stagnation = 0;
        }
        else
            stagnation++;

        new_pop.push_back(BEST);
            
        for(int i = 1; i < INDIVIDUALS; i++) {
            BOT child;
            //if(i % 2) child.symbol = 'X'; else child.symbol = 'O';
            // The child has all the BEST's genomes
            child.symbol = 'X';
            child.genomes = BEST.first.genomes;

            for(auto& [board_state, genome] : pop[i].first.genomes) {
                if(child.genomes.count(board_state)) { // Both parents have this genome
                    // Average of both parent's genomes
                    for(int j = 0; j < 9; j++) {
                        child.genomes[board_state][j] += genome[j];
                        child.genomes[board_state][j] /= 2;
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

        pair<int, pair<int, int>> winrate_table[INDIVIDUALS];
        for (int i = 0; i < INDIVIDUALS; i++){
            winrate_table[i] = {0, {0, 0}};
        }
        for(int j = 0; j < ROUNDS; j++) {
            // This ensures random matchmaking every generation
            shuffle(pop.begin(), pop.end(), rng);

            // Simulates rounds and generates new populations
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
            for (int i = 0; i < INDIVIDUALS; ++i)
            {
                cout << "WIN/DRAW RATE BOT " << i << ": WINS: " << winrate_table[i].first << " DRAWS: " << winrate_table[i].second.first << " LOSSES: " << winrate_table[i].second.second << endl;
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

   void train_population_minimax(bool print = false, bool save_load = false) {
    
    // (Lógica de Carregamento/Inicialização MANTIDA)
    if (save_load) {
        for (int i = 0; i < INDIVIDUALS; ++i) {
            // Assumimos que o load não é mais estritamente atrelado ao símbolo 'X' ou 'O', 
            // mas usamos 'X' para manter a convenção de salvamento.
            string file_name = "X" + to_string(i) + ".txt";
            pop[i].first.load_genomes(file_name); 
            pop[i].first.symbol = 'X'; // Definimos o símbolo base
        }
    } else {
        for (int i = 0; i < INDIVIDUALS; ++i) {
            pop[i].first.symbol = 'X'; // Definimos o símbolo base
        }
    }

    // 1. Instancia o Minimax Player fixo
    Optimal_algorithm fixed_minimax('O'); // O Minimax precisa de um símbolo para inicializar
    
    // Inicialização da Tabela para esta Rodada de ROUNDS
    vector<pair<int, pair<int, int>>> winrate_table(INDIVIDUALS, {0, {0, 0}}); 

    for (int j = 0; j < ROUNDS; j++) {
        
        // Simulates rounds
        for (int i = 0; i < INDIVIDUALS; ++i) { // Iterar sobre todos os BOTs evolutivos
            
            // 2. Cria o controlador, passando o BOT por REFERÊNCIA
            TicTacToeMiniMax game(pop[i].first, fixed_minimax); 

            // --- Jogo 1: BOT é 'X' (Primeiro a jogar) ---
            // 'true' significa que o BOT é 'X'
            int result_x = game.run_game(true, print); 
            
            // Atualiza a pontuação (result_x é do ponto de vista do BOT)
            if (result_x == WIN) {
                pop[i].second += 1;
                winrate_table[i].first++;
            } else if (result_x == LOSS) {
                pop[i].second -= 1;
                winrate_table[i].second.second++;
            } else if (result_x == DRAW) {
                winrate_table[i].second.first++;
            }

            // --- Jogo 2: BOT é 'O' (Segundo a jogar) ---
            // 'false' significa que o BOT é 'O'
            int result_o = game.run_game(false, print); 
            
            // Atualiza a pontuação (result_o é do ponto de vista do BOT)
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
        
        // 3. Impressão e Crossover
        for (int i = 0; i < INDIVIDUALS; ++i)
        {
            cout << "WIN/DRAW RATE BOT " << i << " (Total): WINS: " << winrate_table[i].first 
                 << " DRAWS: " << winrate_table[i].second.first 
                 << " LOSSES: " << winrate_table[i].second.second << endl;
        }

        if (j % CROSSOVER_ROUNDS == 0 && j != 0)
            crossover();
    }
    
    // 4. Salvamento
    if (save_load) {
        BEST.first.save_genomes("BEST.txt");
        for (int i = 0; i < INDIVIDUALS; ++i) {
            string file_name = "X" + to_string(i) + ".txt";
            pop[i].first.save_genomes(file_name);
        }
    }
}

void train_player(bool print = false, bool save_load = false) {
    
    // (Lógica de Carregamento/Inicialização MANTIDA)
    if (save_load) {
        BEST.first.load_genomes("BEST.txt");
    }

    TicTacToePlayer game(BEST.first);
    game.run_game(true, true);
    
    // 4. Salvamento
    if (save_load) {
        BEST.first.save_genomes("BEST.txt");
    }
}
};

int main(void) {
    POPULATION p;

    int opc;
    
    cout << "------------ MENU -------------\n";
    cout << "Choose 1 to train the population\n";
    cout << "Choose 2 to play against the bot BOT\n";
    cin >> opc;

    switch (opc)
    {
    case 1:
        p.train_population_minimax(true,true);
        break;
    
    case 2:
        p.train_player(true, true);
        break;
    
    default:
        break;
    }
    
    return 0;
}